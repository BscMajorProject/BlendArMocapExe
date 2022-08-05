#ifndef CGT_STATE_MACHINE_
#define CGT_STATE_MACHINE_

#include "absl/status/status.h"

namespace BlendArMocap
{
    class StateMachine {
    public:
        StateMachine();

        enum State { IDLE, HAND, FACE, POSE, HOLISTIC, IRIS };
        State GetState();
        absl::Status SetState(State _state);
        absl::Status Reset();

    private:
        bool switching_state;
        State current_state;
        State previous_state;
        absl::Status SwitchState(State designated_state);
    };
}

#endif