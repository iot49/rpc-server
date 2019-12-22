#pragma once

class Task {

    Task(int id);

    // Handle task, return true when completed & result sent to host
    bool run() = 0;

    int get_timeout();
    void set_timeout(int timeout);
}
