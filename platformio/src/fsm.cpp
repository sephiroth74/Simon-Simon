#include "fsm.h"

simon::Fsm::CallbackEnterFunction enter_cb;
simon::Fsm::CallbackExitFunction exit_cb;

String simon::Fsm::stateTypeToString(simon::Fsm::StateType type) {
    switch (type) {
    case simon::Fsm::StateType::INITIAL_STATE:          return F("InitialState");
    case simon::Fsm::StateType::GAME_START_STATE:       return F("GameStartState");
    case simon::Fsm::StateType::PLAYING_SEQUENCE_STATE: return F("PlayingSequenceState");
    case simon::Fsm::StateType::PLAYING_USER_STATE:     return F("PlayingUserState");
    case simon::Fsm::StateType::PLAYING_WIN_STATE:      return F("PlayingWinState");
    case simon::Fsm::StateType::PLAYING_LOSE_STATE:     return F("PlayingLoseState");
    default:                                            return F("UnknownState");
    }
}

String simon::Fsm::eventTypeToString(simon::Fsm::EventType type) {
    switch (type) {
    case simon::Fsm::EventType::GAME_START_EVENT:       return F("GameStart");
    case simon::Fsm::EventType::PLAYING_SEQUENCE_EVENT: return F("PlayingSequence");
    case simon::Fsm::EventType::PLAYING_USER_EVENT:     return F("PlayingUser");
    case simon::Fsm::EventType::PLAYING_WIN_EVENT:      return F("PlayingWin");
    case simon::Fsm::EventType::PLAYING_LOSE_EVENT:     return F("PlayingLose");
    default:                                            return F("UnknownEvent");
    }
}

void simon::Fsm::setEnterCallback(CallbackEnterFunction cb) { enter_cb = cb; }

void simon::Fsm::setExitCallback(CallbackExitFunction cb) { exit_cb = cb; }

void simon::Fsm::dispatchStateEnter(Switch& state) {
    if (enter_cb) {
        enter_cb(state.getType());
    }
}

void simon::Fsm::dispatchStateExit(Switch& state) {
    if (exit_cb) {
        exit_cb(state.getType());
    }
}

void simon::Fsm::Switch::entry() {
    dispatchStateEnter(*this); // Dispatch the enter callback
}

void simon::Fsm::Switch::exit() {
    dispatchStateExit(*this); // Dispatch the exit callback
}

void simon::Fsm::Switch::reset() {
    Serial.println(F("Resetting Game State Machine"));
    // Reset the state machine to the initial state
    ::tinyfsm::StateList<InitialState,
                         GameStartState,
                         PlayingSequenceState,
                         PlayingUserState,
                         PlayingWinState,
                         PlayingLoseState>::reset();
}

simon::Fsm::Switch& simon::Fsm::Switch::currentState() {
    // Return the current state of the FSM
    return *current_state_ptr;
}

void simon::Fsm::InitialState::react(simon::Fsm::Event const& event) {
    Serial.print(F("InitialState: Reacting to event: "));
    Serial.println(simon::Fsm::eventTypeToString(event.type));

    if (event.type == simon::Fsm::EventType::GAME_START_EVENT) {
        transit<simon::Fsm::GameStartState>();
    } else {
        Serial.println(F("Unhandled event in Initial State"));
    }
}

void simon::Fsm::GameStartState::react(simon::Fsm::Event const& event) {
    Serial.print(F("GameStartState: Reacting to event: "));
    Serial.println(simon::Fsm::eventTypeToString(event.type));

    if (event.type == simon::Fsm::EventType::PLAYING_SEQUENCE_EVENT) {
        transit<simon::Fsm::PlayingSequenceState>();
    } else {
        Serial.println(F("Unhandled event in Game Start State"));
    }
}

void simon::Fsm::PlayingSequenceState::react(simon::Fsm::Event const& event) {
    Serial.print(F("PlayingSequenceState: Reacting to event: "));
    Serial.println(simon::Fsm::eventTypeToString(event.type));

    if (event.type == simon::Fsm::EventType::PLAYING_USER_EVENT) {
        transit<simon::Fsm::PlayingUserState>();
    } else {
        Serial.println(F("Unhandled event in Playing Sequence State"));
    }
}

void simon::Fsm::PlayingUserState::react(simon::Fsm::Event const& event) {
    Serial.print(F("PlayingUserState: Reacting to event: "));
    Serial.println(simon::Fsm::eventTypeToString(event.type));

    if (event.type == simon::Fsm::EventType::PLAYING_WIN_EVENT) {
        transit<simon::Fsm::PlayingWinState>();
    } else if (event.type == simon::Fsm::EventType::PLAYING_LOSE_EVENT) {
        transit<simon::Fsm::PlayingLoseState>();
    } else {
        Serial.println(F("Unhandled event in Playing User State"));
    }
}

void simon::Fsm::PlayingWinState::react(simon::Fsm::Event const& event) {
    Serial.print(F("PlayingWinState: Reacting to event: "));
    Serial.println(simon::Fsm::eventTypeToString(event.type));

    if (event.type == simon::Fsm::EventType::PLAYING_SEQUENCE_EVENT) {
        transit<simon::Fsm::PlayingSequenceState>();
    } else {
        Serial.println(F("Unhandled event in Playing Win State"));
    }
}

void simon::Fsm::PlayingLoseState::react(simon::Fsm::Event const& event) {
    Serial.print(F("PlayingLoseState: Reacting to event: "));
    Serial.println(simon::Fsm::eventTypeToString(event.type));

    if (event.type == simon::Fsm::EventType::INITIAL_STATE_EVENT) {
        transit<simon::Fsm::InitialState>();
    } else {
        Serial.println(F("Unhandled event in Playing Lose State"));
    }
}