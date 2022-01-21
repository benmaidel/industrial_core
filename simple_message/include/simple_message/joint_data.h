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

#ifndef JOINT_DATA_H
#define JOINT_DATA_H

#ifndef FLATHEADERS
#include "simple_message/simple_message.h"
#include "simple_message/simple_serialize.h"
#else
#include "simple_message.h"
#include "simple_serialize.h"
#endif

namespace industrial
{
namespace joint_data
{

/**
 * \brief Class encapsulated joint data (positions, accelerations, velocity,
 * torque, and/or effort).
 *
 * For simplicity and cross platform compliance, this is implemented as a
 * fixed size array.
 *
 * The byte representation of a joint data is as follows. The standard sizes
 * are given, but can change based on type sizes:
 *
 *   member:             type                                      size
 *   joints              (float)   4 * MAX_NUM_JOINTS
 *
 *
 * THIS CLASS IS NOT THREAD-SAFE
 *
 */

class JointData : public industrial::simple_serialize::SimpleSerialize
{
public:
  /**
   * \brief Default constructor
   *
   * This method creates empty data.
   *
   */
  JointData(void);
  /**
   * \brief Destructor
   *
   */
  ~JointData(void);

  /**
   * \brief Initializes a empty joint data
   *
   */
  void init();

  /**
   * \brief Sets a joint value within the buffer
   *
   * \param joint index
   * \param joint value
   *
   * \return true if value set, otherwise false (index greater than max)
   */
  bool setJoint(uint32_t index, float value);

  /**
   * \brief Gets a joint value within the buffer
   *
   * \param joint index
   * \param joint value (passed by reference)
   *
   * \return true if value valid, otherwise false (index greater than max)
   */
  bool getJoint(uint32_t index, float & value) const;

  /**
   * \brief Gets a joint value within the buffer (Only use this form if you are
   * sure the index is within bounds).
   *
   * \param joint index
   *
   * \return joint value (returns 0.0 if index is out of bounds)
   */
  float getJoint(uint32_t index) const;

  /**
   * \brief Copies the passed in value
   *
   * \param src (value to copy)
   */
  void copyFrom(JointData &src);

  /**
   * \brief returns the maximum number of joints the message holds
   *
   * \return max number of joints
   */
  uint32_t getMaxNumJoints() const
  {
    return MAX_NUM_JOINTS;
  }

  /**
   * \brief == operator implementation
   *
   * \return true if equal
   */
  bool operator==(JointData &rhs);

  // Overrides - SimpleSerialize
  bool load(industrial::byte_array::ByteArray *buffer);
  bool unload(industrial::byte_array::ByteArray *buffer);
  unsigned int byteLength()
  {
    return MAX_NUM_JOINTS * sizeof(float);
  }

private:

  /**
   * \brief maximum number of joints positions that can be held in the message.
   */
  static const uint32_t MAX_NUM_JOINTS = 10;
  /**
   * \brief internal data buffer
   */
  float joints_[MAX_NUM_JOINTS];

};

}
}

#endif /* JOINT_DATA_H */
