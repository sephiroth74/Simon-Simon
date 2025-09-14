#ifndef __SIMON_FSM_H__
#define __SIMON_FSM_H__

#include <Arduino.h>
#include <functional>
#include <tinyfsm.hpp>

namespace simon {
namespace Fsm {

typedef enum EventType {
    INITIAL_STATE_EVENT,
    GAME_START_EVENT,
    PLAYING_SEQUENCE_EVENT,
    PLAYING_USER_EVENT,
    PLAYING_WIN_EVENT,
    PLAYING_LOSE_EVENT,
} EventType;

typedef enum StateType {
    INITIAL_STATE,
    GAME_START_STATE,
    PLAYING_SEQUENCE_STATE,
    PLAYING_USER_STATE,
    PLAYING_WIN_STATE,
    PLAYING_LOSE_STATE,
} StateType;

String stateTypeToString(StateType type);

String eventTypeToString(EventType type);

struct Event : public ::tinyfsm::Event {
    EventType type;
    Event(EventType t) : type(t) {}
};

struct GameStart : Event {
    GameStart() : Event(EventType::GAME_START_EVENT) {}
};

typedef std::function<void(StateType const&)> CallbackEnterFunction;
typedef std::function<void(StateType const&)> CallbackExitFunction;

struct Switch : public ::tinyfsm::Fsm<Switch> {

  protected:
    StateType type;

  public:
    Switch()  = default;
    ~Switch() = default;

    // State machine methods
    virtual void react(Event const& event);
    virtual void entry();
    virtual void exit();

    StateType getType() const { return type; }

    // Static methods for state management
    static void reset();
    static void init();

    static Switch& currentState();
};

void setEnterCallback(CallbackEnterFunction cb);
void setExitCallback(CallbackExitFunction cb);
void dispatchStateEnter(Switch& state);
void dispatchStateExit(Switch& state);

struct InitialState : public Switch {
    InitialState() { type = StateType::INITIAL_STATE; }
    virtual ~InitialState() = default;
    void react(Event const& event) override;
};

struct GameStartState : public Switch {
    GameStartState() { type = StateType::GAME_START_STATE; }
    virtual ~GameStartState() = default;
    void react(Event const& event) override;
};

struct PlayingSequenceState : public Switch {
    PlayingSequenceState() { type = StateType::PLAYING_SEQUENCE_STATE; }
    virtual ~PlayingSequenceState() = default;
    void react(Event const& event) override;
};

struct PlayingUserState : public Switch {
    PlayingUserState() { type = StateType::PLAYING_USER_STATE; }
    virtual ~PlayingUserState() = default;
    void react(Event const& event) override;
};

struct PlayingWinState : public Switch {
    PlayingWinState() { type = StateType::PLAYING_WIN_STATE; }
    virtual ~PlayingWinState() = default;
    void react(Event const& event) override;
};

struct PlayingLoseState : public Switch {
    PlayingLoseState() { type = StateType::PLAYING_LOSE_STATE; }
    virtual ~PlayingLoseState() = default;
    void react(Event const& event) override;
};

} // namespace Fsm
} // namespace simon

#endif // __SIMON_FSM_H__