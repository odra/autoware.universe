// Copyright 2020 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include "scene_intersection.hpp"
#include "util_type.hpp"

#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/point.hpp>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace behavior_velocity_planner
{
namespace util
{
std::optional<size_t> insertPoint(
  const geometry_msgs::msg::Pose & in_pose,
  autoware_auto_planning_msgs::msg::PathWithLaneId * inout_path);

bool hasLaneIds(
  const autoware_auto_planning_msgs::msg::PathPointWithLaneId & p, const std::set<int> & ids);
std::optional<std::pair<size_t, size_t>> findLaneIdsInterval(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & p, const std::set<int> & ids);

/**
 * @brief get objective polygons for detection area
 */
IntersectionLanelets getObjectiveLanelets(
  lanelet::LaneletMapConstPtr lanelet_map_ptr, lanelet::routing::RoutingGraphPtr routing_graph_ptr,
  const lanelet::ConstLanelet assigned_lanelet, const lanelet::ConstLanelets & lanelets_on_path,
  const std::set<int> & associative_ids, const double detection_area_length,
  const double occlusion_detection_area_length, const bool consider_wrong_direction_vehicle);

/**
 * @brief Generate a stop line for stuck vehicle
 * @param conflicting_areas used to generate stop line for stuck vehicle
 * @param original_path   ego-car lane
 * @param target_path     target lane to insert stop point (part of ego-car lane or same to ego-car
 * lane)
 " @param use_stuck_stopline if true, a stop line is generated at the beginning of intersection lane
 */
std::optional<size_t> generateStuckStopLine(
  const lanelet::CompoundPolygon3d & first_conflicting_area,
  const std::shared_ptr<const PlannerData> & planner_data,
  const InterpolatedPathInfo & interpolated_path_info, const double stop_line_margin,
  const bool use_stuck_stopline, autoware_auto_planning_msgs::msg::PathWithLaneId * original_path);

std::optional<IntersectionStopLines> generateIntersectionStopLines(
  const lanelet::CompoundPolygon3d & first_conflicting_area,
  const lanelet::CompoundPolygon3d & first_detection_area,
  const std::shared_ptr<const PlannerData> & planner_data,
  const InterpolatedPathInfo & interpolated_path_info, const bool use_stuck_stopline,
  const double stop_line_margin, const double peeking_offset,
  autoware_auto_planning_msgs::msg::PathWithLaneId * original_path);

std::optional<size_t> getFirstPointInsidePolygon(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path,
  const std::pair<size_t, size_t> lane_interval, const lanelet::CompoundPolygon3d & polygon,
  const bool search_forward = true);

/**
 * @brief check if ego is over the target_idx. If the index is same, compare the exact pose
 * @param path path
 * @param closest_idx ego's closest index on the path
 * @param current_pose ego's exact pose
 * @return true if ego is over the target_idx
 */
bool isOverTargetIndex(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path, const int closest_idx,
  const geometry_msgs::msg::Pose & current_pose, const int target_idx);

bool isBeforeTargetIndex(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path, const int closest_idx,
  const geometry_msgs::msg::Pose & current_pose, const int target_idx);

/*
lanelet::ConstLanelets extendedAdjacentDirectionLanes(
lanelet::LaneletMapConstPtr map, const lanelet::routing::RoutingGraphPtr routing_graph,
lanelet::ConstLanelet lane);
*/

std::optional<Polygon2d> getIntersectionArea(
  lanelet::ConstLanelet assigned_lane, lanelet::LaneletMapConstPtr lanelet_map_ptr);

bool hasAssociatedTrafficLight(lanelet::ConstLanelet lane);

TrafficPrioritizedLevel getTrafficPrioritizedLevel(
  lanelet::ConstLanelet lane, const std::map<int, TrafficSignalStamped> & tl_infos);

std::vector<DiscretizedLane> generateDetectionLaneDivisions(
  lanelet::ConstLanelets detection_lanelets,
  const lanelet::routing::RoutingGraphPtr routing_graph_ptr, const double resolution);

std::optional<InterpolatedPathInfo> generateInterpolatedPath(
  const int lane_id, const std::set<int> & associative_lane_ids,
  const autoware_auto_planning_msgs::msg::PathWithLaneId & input_path, const double ds,
  const rclcpp::Logger logger);

geometry_msgs::msg::Pose getObjectPoseWithVelocityDirection(
  const autoware_auto_perception_msgs::msg::PredictedObjectKinematics & obj_state);

bool checkStuckVehicleInIntersection(
  const autoware_auto_perception_msgs::msg::PredictedObjects::ConstSharedPtr objects_ptr,
  const Polygon2d & stuck_vehicle_detect_area, const double stuck_vehicle_vel_thr,
  DebugData * debug_data);

Polygon2d generateStuckVehicleDetectAreaPolygon(
  const util::PathLanelets & path_lanelets, const double stuck_vehicle_detect_dist);

bool checkAngleForTargetLanelets(
  const geometry_msgs::msg::Pose & pose, const lanelet::ConstLanelets & target_lanelets,
  const double detection_area_angle_thr, const bool consider_wrong_direction_vehicle,
  const double margin = 0.0);

void cutPredictPathWithDuration(
  autoware_auto_perception_msgs::msg::PredictedObjects * objects_ptr,
  const rclcpp::Clock::SharedPtr clock, const double time_thr);

TimeDistanceArray calcIntersectionPassingTime(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path,
  const std::shared_ptr<const PlannerData> & planner_data, const std::set<int> & associative_ids,
  const size_t closest_idx, const size_t last_intersection_stop_line_candidate_idx,
  const double time_delay, const double intersection_velocity, const double minimum_ego_velocity,
  const bool use_upstream_velocity, const double minimum_upstream_velocity);

double calcDistanceUntilIntersectionLanelet(
  const lanelet::ConstLanelet & assigned_lanelet,
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path, const size_t closest_idx);

std::optional<PathLanelets> generatePathLanelets(
  const lanelet::ConstLanelets & lanelets_on_path,
  const util::InterpolatedPathInfo & interpolated_path_info, const std::set<int> & associative_ids,
  const lanelet::CompoundPolygon3d & first_conflicting_area,
  const std::vector<lanelet::CompoundPolygon3d> & conflicting_areas,
  const std::optional<lanelet::CompoundPolygon3d> & first_attention_area,
  const std::vector<lanelet::CompoundPolygon3d> & attention_areas, const size_t closest_idx,
  const double width);

}  // namespace util
}  // namespace behavior_velocity_planner

#endif  // UTIL_HPP_
