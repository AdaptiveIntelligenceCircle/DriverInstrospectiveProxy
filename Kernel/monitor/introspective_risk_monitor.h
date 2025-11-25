// # DIP/behavior/introspective_risk_monitor.h
#pragma once
#include "../AI/models/anomaly_detector.h"
#include "../AI/models/introspection_risk_model.h"
#include "behavior_state.h"
#include "behavior_bus.h"

#include <memory>
#include <vector>

using namespace std ; 

namespace dip::behavior 
{

class IntrospectiveRiskMonitor 
{
public:
IntrospectiveRiskMonitor() = default;
~IntrospectiveRiskMonitor() = default;

// feed raw numeric features (sensor fusion output)
void feed_features(const vector<double> &features);

// periodic evaluate call (dt in seconds)
void evaluate(double dt);

// attach shared state and bus
void bind(BehaviorState *state, BehaviorBus *bus);

private:
vector<double> buffer_;
driver_ai ::models::IntrospectiveModel risk_model_;
driver_ai :: models::AnomalyDetector anomaly_detector_;
BehaviorState *state_ = nullptr;
BehaviorBus *bus_ = nullptr;
};
} // namespace dip::behavior

