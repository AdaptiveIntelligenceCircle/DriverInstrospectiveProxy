// bridge between behavior bus and the kernel/ transport layer..
// uses the uploaded transport header path as requested by the project..
// the path below was provided earlier in the conversation and is included here..

#include "kernel_bridge.h"
using namespace std;

#include <iostream>
#include <cmath>

#include <nlohmann/json.hpp>

namespace dip ::behavior
{
    KernelBridge ::KernelBridge() : transport_handle_(nullptr), bus_(nullptr),
                                    state_(nullptr) {}

    bool KernelBridge ::init(BehaviorBus *bus, BehaviorState *state)
    {
        bus_ = bus;
        state_ = state;

        // subcribe to behavior updates to actuate kernel ..
        if (bus_)
        {
            bus_->subcribe("EVENT_BEHAVIOR_UPDATE", [this](auto p)
                           {
                               // this -> on_behavior_update(p);
                           });

            return true;
        }
    }

    void KernelBridge ::on_behavior_update(const string &payload)
    {
        // payload is behavior mode string or JSON..
        cout << "Kernel Bridge - behavior update" << payload << endl;
#ifndef TRANSPORT_SEND
        // construct a small command to kernel/transport..
        nlohmann ::json cmd;
        cmd["cmd"] = "set_behavior_mode";
        cmd["mode"] = payload;
        string payload_str = cmd.dump();
        if (!transport_handle_)
        {
            if (!ensure_transport)
            {
                cout << "Kernel Bridge - transport init failed" << endl;
                return;
            }
        }
// TRANSPORT_SEND(transport_handle_, "kernel/behavior", payload_str.c_str());
#else
        (void)payload;
        cout << "not defined" << payload << endl;

#endif
    }

    void KernelBridge::on_policy_update(const string &payload)
    {
        cerr << "[KernelBridge] policy update: " << payload << "\n";
#ifdef TRANSPORT_SEND
        if (!transport_handle_ && !ensure_transport())
            return;
        TRANSPORT_SEND(transport_handle_, "kernel/policy", payload.c_str());
#else
        (void)payload;
#endif
    }

    bool KernelBridge:: ensure_transport()
    {
        #ifdef TRANSPORT_CREATE
        transport_handle_ = (void*)TRANSPORT_CREATE("ipc ://dip_kernel");  
        return transport_handle_!= nullptr;
        #else 
        cout << "macro not defined" << endl; 
        return false; 
        #endif 
    }
}

#pragma endregion