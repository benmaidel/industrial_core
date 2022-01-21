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

#ifndef JOINT_TRAJ_PT_H
#define JOINT_TRAJ_PT_H

#ifndef FLATHEADERS
#include "simple_message/joint_data.h"
#include "simple_message/simple_message.h"
#include "simple_message/simple_serialize.h"
#else
#include "joint_data.h"
#include "simple_message.h"
#include "simple_serialize.h"
#endif

namespace industrial
{
namespace joint_traj_pt
{

namespace SpecialSeqValues
{
enum SpecialSeqValue
{
  START_TRAJECTORY_DOWNLOAD  = -1, ///< Downloading drivers only: signal start of trajectory
  START_TRAJECOTRY_STREAMING = -2, ///< deprecated, please use START_TRAJECTORY_STREAMING instead
  START_TRAJECTORY_STREAMING = -2, ///< Streaming drivers only: signal start of trajectory
  END_TRAJECTORY  = -3, ///< Downloading drivers only: signal end of trajectory
  STOP_TRAJECTORY = -4  ///< Server should stop the current motion (if any) as soon as possible
};
}
typedef SpecialSeqValues::SpecialSeqValue SpecialSeqValue;

/**
 * \brief Class encapsulated joint trajectory point data.  The point data
 * serves as a waypoint along a trajectory and is meant to mirror the
 * JointTrajectoryPoint message.
 *
 * This point differs from the ROS trajectory point in the following ways:
 *
 *  - The joint velocity in an industrial robot standard way (as a single value).
 *  - The duration is somewhat different than the ROS timestamp.  The timestamp
 *    specifies when the move should start, where as the duration is how long the
 *    move should take.  A big assumption is that a sequence of points is continuously
 *    executed.  This is generally true of a ROS trajectory but not required.
 *
 * The byte representation of a joint trajectory point is as follow (in order lowest index
 * to highest). The standard sizes are given, but can change based on type sizes:
 *
 *   member:             type                                      size
 *   sequence            (uint32_t)                                 4  bytes
 *   joints              (industrial::joint_data)                  40 bytes
 *   velocity            (float)                                    4  bytes
 *   duration            (float)                                    4  bytes
 *
 *
 * THIS CLASS IS NOT THREAD-SAFE
 *
 */

class JointTrajPt : public industrial::simple_serialize::SimpleSerialize
{
public:
  /**
   * \brief Default constructor
   *
   * This method creates empty data.
   *
   */
  JointTrajPt(void);
  /**
   * \brief Destructor
   *
   */
  ~JointTrajPt(void);

  /**
   * \brief Initializes a empty joint trajectory point
   *
   */
  void init();

  /**
   * \brief Initializes a complete trajectory point
   *
   */
  void init(uint32_t sequence, industrial::joint_data::JointData & position,
            float velocity, float duration);

  /**
   * \brief Sets joint position data
   *
   * \param joint position data
   */
  void setJointPosition(industrial::joint_data::JointData &position)
  {
    this->joint_position_.copyFrom(position);
  }

  /**
   * \brief Returns a copy of the position data
   *
   * \param joint position dest
   */
  void getJointPosition(industrial::joint_data::JointData &dest)
  {
    dest.copyFrom(this->joint_position_);
  }

  /**
   * \brief Sets joint trajectory point sequence number
   *
   * \param sequence value
   */
  void setSequence(uint32_t sequence)
  {
    this->sequence_ = sequence;
  }

  /**
   * \brief Returns joint trajectory point sequence number
   *
   * \return joint trajectory sequence number
   */
  uint32_t getSequence()
  {
    return this->sequence_;
  }

  /**
   * \brief Sets joint trajectory point velocity
   *
   * \param velocity value
   */
  void setVelocity(float velocity)
  {
    this->velocity_ = velocity;
  }

  /**
   * \brief Returns joint trajectory point velocity
   *
   * \return joint trajectory velocity
   */
  float getVelocity()
  {
    return this->velocity_;
  }

  /**
     * \brief Sets joint trajectory point duration
     *
     * \param velocity value
     */
    void setDuration(float duration)
    {
      this->duration_ = duration;
    }

    /**
     * \brief Returns joint trajectory point duration
     *
     * \return joint trajectory velocity
     */
    float getDuration()
    {
      return this->duration_;
    }

  /**
   * \brief Copies the passed in value
   *
   * \param src (value to copy)
   */
  void copyFrom(JointTrajPt &src);

  /**
   * \brief == operator implementation
   *
   * \return true if equal
   */
  bool operator==(JointTrajPt &rhs);

  // Overrides - SimpleSerialize
  bool load(industrial::byte_array::ByteArray *buffer);
  bool unload(industrial::byte_array::ByteArray *buffer);
  unsigned int byteLength()
  {
    return sizeof(float) + sizeof(uint32_t)
        + this->joint_position_.byteLength();
  }

private:

  /**
   * \brief joint point positional data
   */
  industrial::joint_data::JointData joint_position_;
  /**
   * \brief joint point velocity
   */
  float velocity_;
  /**
   * \brief trajectory sequence number
   */
  uint32_t sequence_;

  /**
   * \brief joint move duration
   */
  float duration_;

};

}
}

#endif /* JOINT_TRAJ_PT_H */
