#pragma once

#include <kf/tui.hpp>

#include "zms/Task.hpp"
#include "zms/ui/pages/MainPage.hpp"

namespace zms {

struct TaskPage : kf::tui::Page {

    Task::State selected{Task::State::GoDist};

    kf::tui::ComboBox<Task::State, 2> task_selector;
    kf::tui::Button start;

    using ArgInput = kf::tui::Labeled<kf::tui::SpinBox<rs::f32>>;
    ArgInput arg_input;
    ArgInput arg_step_input;
    rs::f32 arg_step{1.0f};
    const rs::f32 arg_step_step{0.1f};

    using ResultDisplay = kf::tui::Labeled<kf::tui::Display<rs::u32>>;

    ResultDisplay result_display;

    ArgInput speed_input;

    const rs::f32 speed_step{0.1f};

    TaskPage(Task &task) :
        Page{"Task"},
        start{"Start", [this, &task](kf::tui::Button &) { task.current_state = selected; }},
        task_selector{
            {{
                {"GoDist", Task::State::GoDist},
                {"Turn", Task::State::Turn},
            }},
            selected,
        },
        arg_input{
            "Arg",
            ArgInput::Content{
                task.arg,
                arg_step,
                ArgInput::Content::Mode::Arithmetic}},
        arg_step_input{
            "arg-step",
            ArgInput::Content{
                arg_step,
                arg_step_step,
                ArgInput::Content::Mode::Geometric}},
        result_display{
            "Result",
            ResultDisplay::Content{task.result}},
        speed_input{
            "Speed",
            ArgInput::Content{
                task.speed,
                speed_step,
                ArgInput::Content::Mode::ArithmeticPositiveOnly}}

    {
        link(MainPage::instance());
        add(result_display);
        add(task_selector);
        add(start);
        add(arg_input);
        add(arg_step_input);
        add(speed_input);
    }
};

}// namespace zms
