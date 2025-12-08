-- Driver Introspective Proxy .. 
-- Main entry script... 

local rules = require("Introspection_rules") 
local safety = require("safety_feedback")
local adaptive = require("adaptive_thresholds")
local bh = require("Behavior hints") 

-- Runtime - provided APIs
-- Sensor.get("lidar").distance.. 
-- sensor.get("Camera").distance 

-- system.latency()
-- system.jitter()


local last_inspection = os.clock()

function tick()
    local now = os.clock()
    local dt = now - last_inspection
    last_inspection = now 

    -- Partial snapshot .. 
    local snap = 
    {
        lidar = sensor.get("lidar"),
        camera = sensor.get("camera"),
        gps = sensor.get("gps"),
        imu = sensor.get("imu"),
        state = safety.state(),
        latency = system.latency(),
        jitter = system.jitter()
    }

    -- 1. Apply introspection rules
    local decision = rules.evaluate(snap)

    -- 2. Adjust thresholds adaptively
    adaptive.update(snap)

    -- 3. Inform safety module if necessary
    safety.feedback(snap, decision)

    -- 4. Provide behavior hints to AI logic
    bh.inject(decision, snap)

    -- 5. Logging
    system.log("DIP", {
        decision = decision,
        latency = snap.latency,
        jitter = snap.jitter,
        safety_state = snap.state
    })
end 

return {tick = tick}