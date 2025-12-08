-- ADAPTIVE THRESHOLDS

local M = {}

local history = {
    jitter = {},
    latency = {},
    max_size = 60  -- keep last 60 samples
}

local function push(tbl, v)
    table.insert(tbl, v)
    if #tbl > history.max_size then table.remove(tbl, 1) end
end

function M.update(s)
    -- Store recent metrics
    push(history.jitter, s.jitter)
    push(history.latency, s.latency)

    -- Compute moving average
    local function avg(tbl)
        local sum = 0
        for _,v in ipairs(tbl) do sum = sum + v end
        return sum / math.max(1, #tbl)
    end

    local avg_jitter = avg(history.jitter)
    local avg_latency = avg(history.latency)

    -- Adjust system thresholds (runtime API)
    if avg_jitter > 0.015 then
        system.set_threshold("jitter_limit", avg_jitter * 1.2)
    end

    if avg_latency > 0.040 then
        system.set_threshold("latency_limit", avg_latency * 1.2)
    end
end

return M
