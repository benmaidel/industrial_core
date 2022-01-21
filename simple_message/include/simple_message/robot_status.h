/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Southwest Research Institute
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 	* Redistributions of source code must retain the above copyright
 * 	notice, this list of conditions and the following disclaimer.
 * 	* Redistributions in binary form must reproduce the above copyright
 * 	notice, this list of conditions and the following disclaimer in the
 * 	documentation and/or other materials provided with the distribution.
 * 	* Neither the name of the Southwest Research Institute, nor the names
 *	of its contributors may be used to endorse or promote products derived
 *	from this software without specific prior written permission.
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

#ifndef ROBOT_STATUS_H
#define ROBOT_STATUS_H

#ifndef FLATHEADERS
#include "simple_message/simple_message.h"
#include "simple_message/simple_serialize.h"
#else
#include "simple_message.h"
#include "simple_serialize.h"
#endif

namespace industrial
{
namespace robot_status
{

/**
 * \brief Enumeration mirrors industrial_msgs/RobotMode definition
 *
 */
namespace RobotModes
{
enum RobotMode
{
  UNKNOWN = -1,

  MANUAL = 1, AUTO = 2,
};

#ifdef ROS
int toROSMsgEnum(RobotModes::RobotMode mode);
#endif

}
typedef RobotModes::RobotMode RobotMode;

/**
 * \brief Enumeration mirrors industrial_msgs/TriState definition.
 * NOTE: The TS prefix is needed because the ON and TRUE value collide
 * with other defined types on some systems.
 *
 */
namespace TriStates
{

enum TriState
{
  TS_UNKNOWN = -1,
  // These values must all be the same
  TS_TRUE = 1,   TS_ON = 1,  TS_ENABLED = 1,  TS_HIGH = 1,
  // These values must all be the same
  TS_FALSE = 0,   TS_OFF = 0,  TS_DISABLED = 0,  TS_LOW = 0
};

#ifdef ROS
int toROSMsgEnum(TriStates::TriState state);
#endif

}
typedef TriStates::TriState TriState;

/**
 * \brief Class encapsulated robot status data.  The robot status data is
 * meant to mirror the industrial_msgs/RobotStatus message.
 *
 *
 * The byte representation of a robot status is as follows (in order lowest index
 * to highest). The standard sizes are given, but can change based on type sizes:
 *
 *   member:             type                                      size
 *   drives_powered      (int32_t)    4  bytes
 *   e_stopped           (int32_t)    4  bytes
 *   error_code          (int32_t)    4  bytes
 *   in_error            (int32_t)    4  bytes
 *   in_motion           (int32_t)    4  bytes
 *   mode                (int32_t)    4  bytes
 *   motion_possible     (int32_t)    4  bytes
 *
 * THIS CLASS IS NOT THREAD-SAFE
 *
 */

class RobotStatus : public industrial::simple_serialize::SimpleSerialize
{
public:
/**
 * \brief Default constructor
 *
 * This method creates empty data.
 *
 */
RobotStatus(void);
/**
 * \brief Destructor
 *
 */
~RobotStatus(void);

/**
 * \brief Initializes an empty robot status
 *
 */
void init();

/**
 * \brief Initializes a full robot status message
 *
 */
void init(TriState drivesPowered, TriState eStopped, int32_t errorCode, TriState inError,
          TriState inMotion, RobotMode mode, TriState motionPossible);

TriState getDrivesPowered()
{
  return TriState(drives_powered_);
}

TriState getEStopped()
{
  return TriState(e_stopped_);
}

int32_t getErrorCode() const
{
  return error_code_;
}

TriState getInError()
{
  return TriState(in_error_);
}

TriState getInMotion()
{
  return TriState(in_motion_);
}

RobotMode getMode()
{
  return RobotMode(mode_);
}

TriState getMotionPossible()
{
  return TriState(motion_possible_);
}

void setDrivesPowered(TriState drivesPowered)
{
  this->drives_powered_ = drivesPowered;
}

void setEStopped(TriState eStopped)
{
  this->e_stopped_ = eStopped;
}

void setErrorCode(int32_t errorCode)
{
  this->error_code_ = errorCode;
}

void setInError(TriState inError)
{
  this->in_error_ = inError;
}

void setInMotion(TriState inMotion)
{
  this->in_motion_ = inMotion;
}

void setMode(RobotMode mode)
{
  this->mode_ = mode;
}

void setMotionPossible(TriState motionPossible)
{
  this->motion_possible_ = motionPossible;
}

/**
 * \brief Copies the passed in value
 *
 * \param src (value to copy)
 */
void copyFrom(RobotStatus &src);

/**
 * \brief == operator implementation
 *
 * \return true if equal
 */
bool operator==(RobotStatus &rhs);

// Overrides - SimpleSerialize
bool load(industrial::byte_array::ByteArray *buffer);
bool unload(industrial::byte_array::ByteArray *buffer);
unsigned int byteLength()
{
  return 7 * sizeof(int32_t);
}

private:

/**
 * \brief Operating mode (see RobotModes::RobotMode)
 */
int32_t mode_;

/**
 * \brief E-stop state (see TriStates::TriState)
 */
int32_t e_stopped_;

/**
 * \brief Drive power state (see TriStates::TriState)
 */
int32_t drives_powered_;

/**
 * \brief motion possible state (see TriStates::TriState)
 */
int32_t motion_possible_;

/**
 * \brief in motion state (see TriStates::TriState)
 */
int32_t in_motion_;

/**
 * \brief in error state (see TriStates::TriState)
 */
int32_t in_error_;

/**
 * \brief error code (non-zero is error)
 */
int32_t error_code_;

};

}
}

#endif /* JOINT_TRAJ_PT_H */
