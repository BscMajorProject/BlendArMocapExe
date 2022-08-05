#include "state_machine.h"


namespace BlendArMocap
{
    StateMachine::StateMachine()
    {
        this->previous_state = IDLE;
        this->current_state = IDLE;
        this->switching_state = false;
    }

    StateMachine::State StateMachine::GetState()
    {
        return this->current_state;
    }

    
    absl::Status StateMachine::SwitchState(State designated_state)
    {
        switch(designated_state)
        {
            case IDLE:
            break;

            case HAND:
            break;

            case FACE:
            break;

            case POSE:
            break;

            case HOLISTIC:
            break;

            case IRIS:
            break;
        }

        this->previous_state = this->current_state;
        this->current_state = designated_state;
        return absl::OkStatus();
    }

    absl::Status StateMachine::SetState(State _state)
    {
        if (_state != this->current_state && !this->switching_state) 
        {
            this->switching_state = true;
            absl::Status status = SwitchState(_state);
            if (!status.ok()) { Reset(); }
            this->switching_state = false;
        }

        return absl::OkStatus();
    }

    absl::Status StateMachine::Reset() 
    {
        this->switching_state = true;

        absl::Status status = SwitchState(IDLE);
        if (!status.ok()) { return status; }

        this->switching_state = false;
        return absl::OkStatus();
    }
}