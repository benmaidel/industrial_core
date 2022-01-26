/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Southwest Research Institute
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *       * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *       * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *       * Neither the name of the Southwest Research Institute, nor the names
 *       of its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLATHEADERS
#include "simple_message/log_wrapper.h"
#include "simple_message/smpl_msg_connection.h"
#include "simple_message/byte_array.h"
#else
#include "log_wrapper.h"
#include "smpl_msg_connection.h"
#include "byte_array.h"
#endif

#ifdef MOTOPLUS
#include "motoPlus.h"
#endif

using namespace industrial::simple_message;
using namespace industrial::byte_array;

namespace industrial
{

namespace smpl_msg_connection
{


bool SmplMsgConnection::sendMsg(SimpleMessage & message)
{
  bool rtn;
  ByteArray sendBuffer;
  ByteArray msgData;

  sendBuffer.init();
  if (message.validateMessage())
  {
    //Load the start tag to the byte array
    for(int i = 0; i < message.START_TAG.size(); i++)
      sendBuffer.load((void *)&message.START_TAG[i], sizeof(char));

    message.toByteArray(msgData);
    sendBuffer.load(msgData);
    //Attach the end tag to the end of the byte array
    for(int i = 0; i < message.END_TAG.size(); i++)
      sendBuffer.load((void *)&message.END_TAG[i], sizeof(char));
    ROS_INFO_STREAM("Sending message: "<< std::endl << message);
    rtn = this->sendBytes(sendBuffer);
  }
  else
  {
    rtn = false;
    LOG_ERROR("Message validation failed, message not sent");
  }

return rtn;
}


bool SmplMsgConnection::receiveMsg(SimpleMessage &message)
{
  ByteArray tagBuffer;
  ByteArray headerBuffer;
  ByteArray msgBuffer;
  int length;

  bool rtn = false;

  // receive the start tag
  rtn = this->receiveBytes(tagBuffer, 4);

  if (rtn)
  {
    std::vector<char> tag;
    tagBuffer.copyTo(tag);
    //validate the start tag
    if(!(message.START_TAG == tag))
    {
      LOG_ERROR("Received message did not start with the correct start tag.");
      return false;
    }

    //receive the header
    rtn = this->receiveBytes(headerBuffer, message.getHeaderSize());
    if (rtn)
    {
      rtn = message.init(headerBuffer);
      if (rtn)
      {
        ROS_INFO_STREAM("received header: " <<std::endl << message);
        if(message.getPayloadLength() > 0)
        {
          rtn = this->receiveBytes(msgBuffer, message.getPayloadLength());
          if (rtn)
          {
            message.setData(msgBuffer);

          }
          else
          {
            LOG_ERROR("Failed to receive message payload");
            rtn = false;
          }
        }
        if (rtn)
        {
          //receive the end tag and validate it
          tagBuffer.init();
          rtn = this->receiveBytes(tagBuffer, 4);
          tag.clear();
          tagBuffer.copyTo(tag);

          if(!(message.END_TAG == tag))
          {
            LOG_ERROR("Received message did not end with the correct end tag.");
            rtn = false;
          }
        }
      }
      else
      {
        LOG_ERROR("Message header initialization failed");
        rtn = false;
      }
    }
    else
    {
      LOG_ERROR("Failed to receive header");
      rtn = false;
    }
  }
  else
  {
    LOG_ERROR("Failed to receive start tag");
    rtn = false;
  }

  return rtn;
}



bool SmplMsgConnection::sendAndReceiveMsg(SimpleMessage & send, SimpleMessage & recv, bool verbose)
{
  bool rtn = false;
  rtn = this->sendMsg(send);
  if (rtn)
  {
    if(verbose) {
      LOG_ERROR("Sent message");
    }
    rtn = this->receiveMsg(recv);
    if(verbose) {
      LOG_ERROR("Got message");
    }
  }
  else
  {
    rtn = false;
  }

  return rtn;
}


}//smpl_msg_connection
}//industrial
