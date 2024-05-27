#include "bn_seed_random.h"
#include "bn_core.h"
#include "bn_sprite_text_generator.h"
#include "bn_time.h"
#include "bn_optional.h"
#include "bn_log.h"
#include "bn_keypad.h"

void displayInfo()
{
    BN_LOG("Bullets: ", noOfBullets - curBullet, "\n\t\tPlayer HP: ", playerHP, "\n\t\tDealerHP: ", dealerHP);
}

void game(bn::sprite_text_generator &text_generator)
{
    text_generator.set_center_alignment();

    bn::seed_random rand;
    bn::optional<bn::time> time = bn::time::current();
    int seed = time->hour() * 10000 + time->minute() * 100 + time->second();
    BN_LOG("SEED: ",seed);
    rand.set_seed(seed);

    int noOfBullets = rand.get_unbiased_int(2, 9);
    int lowlimit = noOfBullets > 4 ? 2 : 1;
    int uplimit = noOfBullets > 4 ? noOfBullets - 1 : noOfBullets;
    int noOfLiveBullets = rand.get_unbiased_int(lowlimit, uplimit);
    bool *bullets = new bool[noOfBullets];

    for (int i = 0; i < noOfLiveBullets; i++)
    {
        bullets[i] = true;
    }

    // shuffle bullet array
    for (int i = 0; i < noOfBullets; i++)
    {
        int j = rand.get_unbiased_int(3);
        bool temp = bullets[i];
        bullets[i] = bullets[j];
        bullets[j] = temp;
    }

    for (int i = 0; i < noOfBullets; i++)
    {
        BN_LOG(bullets[i]);
    }

    BN_LOG(noOfLiveBullets, noOfBullets);

    bool gameOver = false;
    int curBullet = 0;
    bool canInput = true;
    while (!gameOver && canInput)
    {
        // Shoot Dealer
        if (bn::keypad::a_pressed())
        {
            if (bullets[curBullet])
            {
                dealerHP--;
                // dealer plays
            }
            else
            {
                // lose turn, dealer plays
            }
            curBullet++;
            displayInfo();
        }

        // Shoot Yourself
        else if (bn::keypad::b_pressed())
        {
            if (bullets[curBullet])
            {
                playerHP--;
                // dealer plays
            }
            curBullet++;
            displayInfo();
        }

        if (curBullet == noOfBullets || playerHP <= 0 || dealerHP <= 0)
        {
            gameOver = true;
            displayInfo();
        }

        bn::core::update();
    }
}