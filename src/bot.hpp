//
// Created by Thomas Lienbacher on 25.04.2019.
//

#ifndef COCBOT_BOT_HPP
#define COCBOT_BOT_HPP


class Bot {

public:
    Bot();

    ~Bot();

    Bot(const Bot &other) = delete;

    Bot(Bot &&other) noexcept = delete;

    Bot &operator=(const Bot &other) = delete;

    Bot &operator=(Bot &&other) noexcept = delete;

    bool process();

    void requestBaseCenter();
};


#endif //COCBOT_BOT_HPP
