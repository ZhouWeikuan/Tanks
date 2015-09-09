#ifndef APPTHREAD
#define APPTHREAD
#include "event/eventstore.h"
#include "shareddata.h"
#include "game/objects/object.h"

#include <process.h>
#include <windows.h>

/**
 * @brief
 * Klasa bazowa dla dwóch wątków w aplikacji: gry i sieci.
 */
class AppThread
{
public:
    AppThread(SharedData* shared_data, CRITICAL_SECTION* critical_section);

    /**
     * Uruchomienie działania wątku.
     */
    virtual void run() = 0;
    /**
     * Czy gra jest w sieci.
     * @return
     */
    boolean isNetworkRunning();

    /**
     * Dane współdzielone między wątkami.
     */
    SharedData* shared_data;

    /**
     * Blokada wątku wykorzystywana przy dostępie do współdzielonych danych.
     */
    CRITICAL_SECTION* critical_section;

    /**
     * Pobranie obecnej ramki gry.
     * @return
     */
    unsigned long getCurrentFrame()
    {
        unsigned long frame;
        EnterCriticalSection(critical_section);
            frame = shared_data->current_frame_number;
        LeaveCriticalSection(critical_section);

        return frame;
    }

    /**
     * Pobranie identyfikatora klienta.
     * @return
     */
    unsigned long getPlayerId()
    {
        unsigned long player_id;
        EnterCriticalSection(critical_section);
            player_id = shared_data->player_id;
        LeaveCriticalSection(critical_section);

        return player_id;
    }

    /**
     * Wysyłanie pozycji danego obiektu
     * @param obj
     * @param obj_type
     * @param fast
     */
    void sendObjectPosition(Object* obj, PositionEvent::PosObj obj_type, bool fast = false)
    {
        PositionEvent* pos = new PositionEvent;
        pos->obj = PositionEvent::PosObj::TANK; //obj_type;
        pos->obj_id.l_value = obj->object_id;
        pos->pos_x.d_value = obj->pos_x;
        pos->pos_y.d_value = obj->pos_y;

        EnterCriticalSection(critical_section);
            shared_data->transmit_events.addEvent(pos);
        LeaveCriticalSection(critical_section);
    }
};

#endif // APPTHREAD

