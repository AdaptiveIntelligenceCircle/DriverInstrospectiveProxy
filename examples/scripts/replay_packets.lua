local json = require("json")

local seq = json.load_file("../anomaly_sequence.json")
for i,frame in ipairs(seq.sequence) do
    print("[Replay] Wheel:", frame.wheel_speed, "| Lidar:", frame.lidar)
end
