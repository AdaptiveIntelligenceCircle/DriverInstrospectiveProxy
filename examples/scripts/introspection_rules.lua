-- INTROSPECTION RULES

local M = {}

-- Example decision output:
-- "normal", "caution", "critical", "failover"

function M.evaluate(s)
    -- Basic heuristics

    if s.lidar.distance < 5 or s.camera.distance < 5 then
        return "critical"
    end

    if s.state == "EMERGENCY" then
        return "failover"
    end

    if s.jitter > 0.020 then
        return "caution"
    end

    if s.latency > 0.050 then
        return "caution"
    end

    return "normal"
end

return M
