-- BEHAVIOR HINTS

local M = {}

function M.inject(decision, s)
    if decision == "critical" then
        control.send({ mode = "BRAKE_SOFT" })
        return
    end

    if decision == "caution" then
        control.send({ mode = "REDUCE_SPEED", factor = 0.8 })
        return
    end

    if decision == "normal" then
        control.send({ mode = "MAINTAIN" })
        return
    end

    if decision == "failover" then
        control.send({ mode = "STOP_IMMEDIATELY" })
        return
    end
end

return M
