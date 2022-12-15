/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <array>
#include <iostream>

#include "csv.h"
#include "eyeGazeReader.h"

namespace ark::datatools {

constexpr std::array<const char*, 5> EyeGazeColumns =
    {"tracking_timestamp_us", "gaze_vector_x", "gaze_vector_y", "gaze_vector_z", "uncertainty"};

TemporalEyeGazeData readEyeGaze(const std::filesystem::path& path) {
  io::CSVReader<EyeGazeColumns.size()> csv(path.string());
  // Read in the CSV header
  const auto readHeader = [&](auto&&... args) { csv.read_header(io::ignore_no_column, args...); };
  std::apply(readHeader, EyeGazeColumns);

  TemporalEyeGazeData eyeGazeSequence;
  // Read each row and populate the trajectory with each recording
  EyeGaze eyeGaze;

  std::int64_t tracking_timestamp_us;

  while (csv.read_row(
      tracking_timestamp_us,
      eyeGaze.gaze_vector.x(),
      eyeGaze.gaze_vector.y(),
      eyeGaze.gaze_vector.z(),
      eyeGaze.uncertainty)) {
    eyeGazeSequence[std::chrono::microseconds(tracking_timestamp_us)] = eyeGaze;
    eyeGaze = {}; // reset gaze data for the next reading
  }
  std::cout << "Loaded #eyegaze records: " << eyeGazeSequence.size() << std::endl;
  return eyeGazeSequence;
}

} // namespace ark::datatools
