local M = {}

function M.feedback(s, decision)
    -- Notify safety only when needed.. 
    if decision == "critical" then 
        safety.alert("CRITICAL_PROXIMITY", {
            lidar = s.lidar.distance, 
            camera = s.camera.distance 
        })
    end 

    if s.jitter > 0.03 then 
        safety.alert("High_JITTER", {
            jitter = s.jitter
        })
    end 

    if s.latency > 0.08 then 
        safety.alert("RUNTIME_LATENCY_SPIKE", { latency = s.latency })
    end 
end 

return M 