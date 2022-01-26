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
#include "simple_message/simple_message.h"
#include "simple_message/log_wrapper.h"
#else
#include "simple_message.h"
#include "log_wrapper.h"
#endif

#ifdef MOTOPLUS
#include "motoPlus.h"
#endif


using namespace industrial::byte_array;

namespace industrial
{

namespace simple_message
{

SimpleMessage::SimpleMessage(void)
{
}

SimpleMessage::~SimpleMessage(void)
{
}



bool SimpleMessage::init(uint16_t msgType, uint8_t versionMajor, uint8_t versionMinor, uint8_t commType, uint8_t replyCode)
{
  ByteArray data;
  data.init();
  return this->init(msgType, versionMajor, versionMinor, commType, replyCode, data);
}

bool SimpleMessage::init(uint16_t msgType, uint8_t versionMajor, uint8_t versionMinor, uint8_t commType, uint8_t replyCode,
                      ByteArray & data )
{
  LOG_COMM("SimpleMessage::init(type: %d, comm: %d, reply: %d, data[%d]...)",
            msgType, commType, replyCode, data.getBufferSize());
  this->setMessageType(msgType);
  memset(&this->message_string_, 0, MESSAGE_STRING_LENGTH);
  this->setVersionMajor(versionMajor);
  this->setVersionMinor(versionMinor);
  this->setCommType(commType);
  this->setReplyCode(replyCode);
  this->setSequence(0);
  this->setTimeStamp(0, 0);
  this->data_.copyFrom(data);
  this->payload_length_ = this->data_.getBufferSize();

  return this->validateMessage();
}

bool SimpleMessage::init(ByteArray & msg)
{
  bool rtn = false;

  if (msg.getBufferSize() >= this->getHeaderSize())
  {
    // Check to see if the message is larger than the standard header
    // If so, copy out the data portion.
    LOG_COMM("Unloading header data");
    msg.unloadFront(this->payload_length_);
    msg.unloadFront(this->message_type_);
    msg.unloadFront(&this->message_string_, MESSAGE_STRING_LENGTH * sizeof(char));
    msg.unloadFront(this->version_major_);
    msg.unloadFront(this->version_minor_);
    msg.unloadFront(this->sequence_);
    msg.unloadFront(this->timestamp_sec_);
    msg.unloadFront(this->timestamp_nsec_);
    msg.unloadFront(this->comm_type_);
    msg.unloadFront(this->reply_code_);
    LOG_COMM("SimpleMessage::init(msg_length: %d, msg_type: %d, version_major: %d, version_minor: %d, msg_comm: %d, msg_reply: %d, data[%d]...)",
              this->payload_length_, this->message_type_, this->version_major_, this->version_minor_, this->comm_type_, this->reply_code_);

    if (msg.getBufferSize() >= this->payload_length_)
    {
      LOG_COMM("Unloading data");
      msg.unloadFront(this->data_, (int)this->payload_length_);
    }

    rtn = this->validateMessage();
  }
  else
  {
    LOG_ERROR("Failed to init message, buffer size too small: %u", msg.getBufferSize());
    rtn = false;
  }
  return rtn;
}

void SimpleMessage::toByteArray(ByteArray & msg)
{
  msg.init();

  msg.load((uint32_t)this->data_.getBufferSize());
  msg.load((uint16_t)this->getMessageType());
  msg.load(&this->message_string_, MESSAGE_STRING_LENGTH * sizeof(char));
  msg.load((uint8_t)this->getVersionMajor());
  msg.load((uint8_t)this->getVersionMinor());
  msg.load((uint16_t)this->getSequence());
  msg.load((uint32_t)this->getTimestampSec());
  msg.load((uint32_t)this->getTimestampNSec());
  msg.load(this->getCommType());
  msg.load(this->getReplyCode());
  if (this->data_.getBufferSize() > 0 )
  {
    msg.load(this->data_);
  }
}


void SimpleMessage::setData( ByteArray & data)
{
  this->data_.copyFrom(data);
}


bool SimpleMessage::validateMessage()
{

  if ( StandardMsgType::INVALID == this->getMessageType())
  {
    LOG_WARN("Invalid message type: %u", this->getMessageType());
    return false;
  }

  if ( CommTypes::INVALID == this->getCommType())
  {
    LOG_WARN("Invalid comms. type: %u", this->getCommType());
    return false;
  }

  if (
      (CommTypes::SERVICE_REPLY ==  this->getCommType() &&
          ReplyTypes::INVALID == this->getReplyCode()) ||
          ((CommTypes::SERVICE_REPLY !=  this->getCommType() &&
              ReplyTypes::INVALID != this->getReplyCode()))
  )
  {
    LOG_WARN("Invalid reply. Comm type: %u, Reply type: %u",
             this->getCommType(), this->getReplyCode());
    return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, const SimpleMessage& msg)
{
    os << "payload_length: " << (int)msg.payload_length_ << "\n";
    os << "message_type: " << (int)msg.message_type_ << "\n";
    os << "message_string: " << std::string(msg.message_string_) << "\n";
    os << "version_major: " << (int)msg.version_major_ << "\n";
    os << "version_minor: " << (int)msg.version_minor_ << "\n";
    os << "seq: " << (int)msg.sequence_ << "\n";
    os << "timestamp_sec: " << (int)msg.timestamp_sec_ << "\n";
    os << "timestamp_nsec: " << (int)msg.timestamp_nsec_ << "\n";
    os << "comm_type: " << (int)msg.comm_type_ << "\n";
    os << "reply_type: " << (int)msg.reply_code_;
    return os;
}

} // namespace simple_message
} // namespace industrial
