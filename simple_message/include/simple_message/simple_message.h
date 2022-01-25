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

#ifndef SIMPLE_MSG_H
#define SIMPLE_MSG_H

#ifndef FLATHEADERS
#include "simple_message/simple_serialize.h"
#include "simple_message/byte_array.h"
#else
#include "simple_serialize.h"
#include "byte_array.h"
#endif

#include <cstddef>

namespace industrial
{

namespace simple_message
{

/**
 * \brief Enumeration of standard message types (supported by all platforms).
 * In addition, each robot interface will support it's own message types.
 */
namespace StandardMsgTypes
{
  enum StandardMsgType
  {
 INVALID = 0,
 PING = 1,

 //TODO: Keeping these message type for the time being.  Refactoring
 // the messages should remove the need for this message.
 JOINT_POSITION = 10,
 JOINT = 10,
 READ_INPUT = 20,
 WRITE_OUTPUT = 21,

 JOINT_TRAJ_PT = 11,  //Joint trajectory point message (typically for streaming)
 JOINT_TRAJ = 12,	  //Joint trajectory message (typically for trajectory downloading)
 STATUS = 13,         //Robot status message (for reporting the robot state)
 JOINT_TRAJ_PT_FULL = 14,  // Joint trajectory point message (all message fields)
 JOINT_FEEDBACK = 15,      // Feedback of joint pos/vel/accel

 // Begin vendor specific message types (only define the beginning enum value,
 // specific enum values should be defined locally, within in the range reserved
 // here.  Each vendor can reserve up 100 types

 SWRI_MSG_BEGIN     = 1000,
 UR_MSG_BEGIN       = 1100,
 ADEPT_MSG_BEGIN    = 1200,
 ABB_MSG_BEGIN      = 1300,
 FANUC_MSG_BEGIN    = 1400,
 MOTOMAN_MSG_BEGIN  = 2000
  };
}
typedef StandardMsgTypes::StandardMsgType StandardMsgType;

/**
 * \brief Enumeration of communications types (supported by all platforms).
 */
namespace CommTypes
{
  enum CommType
  {
 INVALID = 0,
 TOPIC = 1,
 SERVICE_REQUEST = 2,
 SERVICE_REPLY = 3
  };
}
typedef CommTypes::CommType CommType;

/**
 * \brief Enumeration of reply types (supported by all platforms).  In cases of
 * success or failure, the return data should include the relevant return info.
 */
namespace ReplyTypes
{
  enum ReplyType
  {
 INVALID = 0,
 SUCCESS = 1,
 FAILURE = 2
  };
}
typedef ReplyTypes::ReplyType ReplyType;



/**
* \brief This class defines a simple messaging protocol for communicating with an
* industrial robot controller.
*
* The protocol meets the following requirements:
*
* 1. Format should be simple enough that code can be shared between ROS and
* the controller (for those controllers that support C/C++).  For those controllers
* that do not support C/C++, the protocol must be simple enough to be decoded with
* the limited capabilities of the typical robot programming language.  A corollary
* to this requirement is that the protocol should not be so onerous as to overwhelm
* the limited resources of the robot controller
*
* 2. Format should allow for data streaming (ROS topic like)
*
* 3. Format should allow for data reply (ROS service like)
*
* 4. The protocol is not intended to encapsulate version information  It is up to
* individual developers to ensure that code developed for communicating platforms
* does not have any version conflicts (this includes message type identifiers).
*
* Message Structure
*
* - <PREFIX> Not considered part of the message
*   - int LENGTH (HEADER + DATA) in bytes
*
*
*
* - <HEADER>
*   - int MSG_TYPE identifies type of message (standard (see StandardMsgTypes::StandardMsgType)
*     and robot specific values)
*   - int COMM_TYPE identified communications type (see CommTypes::CommType)
*   - int REPLY CODE (service reply only) reply code (see ReplyTypes::ReplyType)
*
* - <BODY>
*   - ByteArray DATA variable length data determined by message
*     type and and communications type.
*
*
* THIS CLASS IS NOT THREAD-SAFE
*
*/
class SimpleMessage
{


public:
  /**
   * \brief Constructs an empty message
   */
	SimpleMessage();

  /**
   * \brief Destructs a message
   */
	~SimpleMessage(void);

  bool init(uint16_t msgType, uint8_t versionMajor, uint8_t versionMinor, uint8_t commType, uint8_t replyCode);

	bool init(uint16_t msgType, uint8_t versionMajor, uint8_t versionMinor, uint8_t commType, uint8_t replyCode,
          industrial::byte_array::ByteArray &data );

  bool init(industrial::byte_array::ByteArray &msg);

  void toByteArray(industrial::byte_array::ByteArray &msg);

  /**
   * \brief Gets size of message header in bytes(fixed)
   *
   * \return message header size
   */
  static uint32_t getHeaderSize() { return SimpleMessage::HEADER_SIZE; };

  /**
   * \brief Gets message type(see StandardMsgType)
   *
   * \return message type
   */
	uint16_t getMessageType() {return this->message_type_;};

  /**
   * \brief Gets message type(see StandardMsgType)
   *
   * \return message type
   */
	std::string getMessageString() {return this->message_string_;};


  /**
   * \brief Gets message type(see CommType)
   *
   * \return communications type
   */
	uint8_t getVersionMajor() {return this->version_major_;};

  /**
   * \brief Gets message type(see CommType)
   *
   * \return communications type
   */
	uint8_t getVersionMinor() {return this->version_minor_;};


  /**
   * \brief Gets message type(see CommType)
   *
   * \return communications type
   */
	uint8_t getSequence() {return this->sequence_;};

  /**
   * \brief Gets message type(see CommType)
   *
   * \return communications type
   */
	uint8_t getCommType() {return this->comm_type_;};

  /**
   * \brief Gets message type(see CommType)
   *
   * \return communications type
   */
	uint8_t getTimestampSec() {return this->timestamp_sec_;};

  /**
   * \brief Gets message type(see CommType)
   *
   * \return communications type
   */
	uint8_t getTimestampNSec() {return this->timestamp_nsec_;};

    /**
   * \brief Gets reply code(see ReplyType)
   *
   * \return reply code
   */
  uint8_t getReplyCode() {return this->reply_code_;};

  /**
   * \brief Gets message length (total size, HEADER + data)
   *
   * \return message length
   */
	uint32_t getMsgLength() {return this->getHeaderSize() + this->data_.getBufferSize();};

  /**
   * \brief Gets length of message data portion.
   *
   * \return message data length
   */
	uint32_t getDataBufferLength() {return this->data_.getBufferSize();};

  uint32_t getPayloadLength() {return this->payload_length_;};

  /**
   * \brief Returns a reference to the internal data member
   *
   * \return reference to message data portion.
   */
  industrial::byte_array::ByteArray & getData() {return this->data_;};

  /**
   * \brief Sets data portion
   *
   * \param data portion
   */
  void setData(industrial::byte_array::ByteArray &data);

  /**
   * \brief performs logical checks to ensure that the message is fully
   * defined and adheres to the message conventions.
   *
   * \return true if message valid, false otherwise
   */
  bool validateMessage();

  const char START_TAG[4] = {0x02, 0x02, 0x02, 0x02};
  const char END_TAG[4] = {0x03, 0x03, 0x03, 0x03};

private:

  uint32_t payload_length_;

  uint16_t message_type_;

  static const uint32_t MESSAGE_STRING_LENGTH = 10;
  std::string message_string_;

  uint8_t version_major_;

  uint8_t version_minor_;

  uint16_t sequence_;

  uint32_t timestamp_sec_;
  uint32_t timestamp_nsec_;

  /**
   * \brief Communications type(see CommType)
   */
  uint8_t comm_type_;

  /**
   * \brief Reply code(see ReplyType)
   */
  uint8_t reply_code_;

  /**
   * \brief Message data portion
   */
	industrial::byte_array::ByteArray data_;

  /**
   * \brief Size(in bytes) of message header (fixed)
   */
	static const uint32_t HEADER_SIZE =
      sizeof(uint32_t) +  //payload_length
      sizeof(uint16_t) +  //message_type
      sizeof(char) * 10 + //message_string
      sizeof(uint8_t) +   //version_major
      sizeof(uint8_t) +   //version_minor
      sizeof(uint16_t) +  //seq
      sizeof(uint32_t) +  //timestamp_sec_
      sizeof(uint32_t) +  //timestamp_nsec_
      sizeof(uint8_t) +   //comm_type_
      sizeof(uint8_t);    //reply_code_

  /**
   * \brief Sets message type
   *
   * \param message type
   */
  void setMessageType(uint8_t msgType) {this->message_type_ = msgType;};

  void setVersionMajor(uint8_t versionMajor) {this->version_major_ = versionMajor;};

  void setVersionMinor(uint8_t versionMinor) {this->version_minor_ = versionMinor;};

  void setSequence(uint16_t sequence) {this->sequence_ = sequence;};

  void setTimeStamp(uint32_t sec, uint32_t nsec) {this->timestamp_sec_ = sec; this->timestamp_nsec_ = nsec;};

  /**
   * \brief Sets communications type
   *
   * \param communications type
   */
  void setCommType(uint8_t commType) {this->comm_type_ = commType;};

  /**
   * \brief Sets reply code
   *
   * \param reply code
   */
  void setReplyCode(uint8_t replyCode) {this->reply_code_ = replyCode;};
};

}//namespace simple_message
}//namespace industrial

#endif //SIMPLE_MSG_
