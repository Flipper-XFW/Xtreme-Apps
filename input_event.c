#include "input_event.h"

void submenu_callback(void *context, uint32_t index)
{
    FlizzerTrackerApp *tracker = (FlizzerTrackerApp *)context;

    switch (tracker->mode)
    {
        case PATTERN_VIEW:
        {
            switch (index)
            {
                case SUBMENU_PATTERN_EXIT:
                {
                    tracker->quit = true;

                    static InputEvent inevent = {.sequence = 0, .key = InputKeyLeft, .type = InputTypeMAX};
                    FlizzerTrackerEvent event = {.type = EventTypeInput, .input = inevent, .period = 0}; // making an event so tracker does not wait for next keypress and exits immediately
                    furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
                    view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
                    break;
                }

                default:
                    break;
            }

            break;
        }

        case INST_EDITOR_VIEW:
        {
            switch (index)
            {
                case SUBMENU_INSTRUMENT_EXIT:
                {
                    tracker->quit = true;

                    static InputEvent inevent = {.sequence = 0, .key = InputKeyLeft, .type = InputTypeMAX};
                    FlizzerTrackerEvent event = {.type = EventTypeInput, .input = inevent, .period = 0}; // making an event so tracker does not wait for next keypress and exits immediately
                    furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
                    view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
                    break;
                }

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }
}

void cycle_focus(FlizzerTrackerApp *tracker)
{
    switch (tracker->mode)
    {
        case PATTERN_VIEW:
        {
            tracker->focus++;

            if (tracker->focus > EDIT_SONGINFO)
            {
                tracker->focus = EDIT_PATTERN;
            }

            break;
        }

        case INST_EDITOR_VIEW:
        {
            tracker->focus++;

            if (tracker->focus > EDIT_PROGRAM)
            {
                tracker->focus = EDIT_INSTRUMENT;
            }

            break;
        }

        default:
            break;
    }
}

void cycle_view(FlizzerTrackerApp *tracker)
{
    if (tracker->mode == PATTERN_VIEW)
    {
        tracker->mode = INST_EDITOR_VIEW;
        tracker->focus = EDIT_INSTRUMENT;

        tracker->selected_param = 0;
        tracker->current_digit = 0;

        return;
    }

    if (tracker->mode == INST_EDITOR_VIEW)
    {
        tracker->mode = PATTERN_VIEW;
        tracker->focus = EDIT_PATTERN;

        if (tracker->tracker_engine.song == NULL)
        {
            stop_song(tracker);
            tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
        }

        tracker->selected_param = 0;
        tracker->current_digit = 0;

        return;
    }
}

void process_input_event(FlizzerTrackerApp *tracker, FlizzerTrackerEvent *event)
{
    if (event->input.key == InputKeyBack && event->input.type == InputTypeShort && event->period > 0 && event->period < 300 && !(tracker->editing))
    {
        cycle_view(tracker);
        return;
    }

    else if (event->input.key == InputKeyBack && event->input.type == InputTypeShort && !(tracker->editing))
    {
        cycle_focus(tracker);
        return;
    }

    // Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
    if (event->input.key == InputKeyBack && event->input.type == InputTypeLong)
    {
        switch (tracker->mode)
        {
            case PATTERN_VIEW:
            {
                view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_SUBMENU_PATTERN);
                break;
            }

            case INST_EDITOR_VIEW:
            {
                view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_SUBMENU_INSTRUMENT);
                break;
            }

            default:
                break;
        }

        return;
    }

    switch (tracker->focus)
    {
        case EDIT_PATTERN:
        {
            pattern_edit_event(tracker, event);
            break;
        }

        case EDIT_SEQUENCE:
        {
            sequence_edit_event(tracker, event);
            break;
        }

        case EDIT_SONGINFO:
        {
            songinfo_edit_event(tracker, event);
            break;
        }

        case EDIT_INSTRUMENT:
        {
            instrument_edit_event(tracker, event);
            break;
        }

        case EDIT_PROGRAM:
        {
            instrument_program_edit_event(tracker, event);
            break;
        }

        default:
            break;
    }
}