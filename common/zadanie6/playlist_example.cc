#include <iostream>
#include "lib_playlist.h"

int main() {
    Player player;

    auto mishmash = player.createPlaylist("mishmash");
    auto armstrong = player.createPlaylist("armstrong");
    auto whatAWonderfulWorld = player.openFile(File("audio|artist:Louis Armstrong|title:What a Wonderful World|"
                                                            "I see trees of green, red roses too..."));
    auto helloDolly = player.openFile(File("audio|artist:Louis Armstrong|title:Hello, Dolly!|"
                                                   "Hello, Dolly! This is Louis, Dolly"));
    armstrong->add(whatAWonderfulWorld);
    armstrong->add(helloDolly);
    auto direstraits = player.openFile(File("audio|artist:Dire Straits|title:Money for Nothing|"
                                                    "Now look at them yo-yo's that's the way you do it..."));
    auto cabaret = player.openFile(File("video|title:Cabaret|year:1972|Qvfcynlvat Pnonerg"));


    mishmash->add(cabaret);
    mishmash->add(armstrong);
    mishmash->add(direstraits, 1);
    mishmash->add(direstraits);

    std::cout << "=== Playing 'mishmash' (default sequence mode)" << std::endl;
    mishmash->play();

    std::cout << "=== Playing 'mishmash' (shuffle mode, seed 0 for std::default_random_engine)" << std::endl;
    mishmash->setMode(createShuffleMode(0));
    mishmash->play();

    std::cout << "=== Playing 'mishmash' (removed cabaret and last direstraits, odd-even mode)" << std::endl;
    mishmash->remove(0);
    mishmash->remove();
    mishmash->setMode(createOddEvenMode());
    mishmash->play();

    std::cout << "=== Playing 'mishmash' (sequence mode, 'armstrong' odd-even mode)" << std::endl;
    armstrong->setMode(createOddEvenMode());
    mishmash->setMode(createSequenceMode());
    mishmash->play();

    try {
        auto unsupported = player.openFile(File("mp3|artist:Unsupported|title:Unsupported|Content"));
    } catch (PlayerException const& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        auto corrupted = player.openFile(File("Corrupt"));
    } catch (PlayerException const& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        auto corrupted = player.openFile(File("audio|artist:Louis Armstrong|title:Hello, Dolly!|%#!@*&"));
    } catch (PlayerException const& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
