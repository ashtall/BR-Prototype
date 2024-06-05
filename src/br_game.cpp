#include "bn_log.h"
#include "bn_core.h"
#include "bn_math.h"
#include "bn_time.h"
#include "bn_fixed.h"
#include "bn_keypad.h"
#include "br_globals.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_seed_random.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_items_test_dealer.h"

int curBullet = 0;
bool *bullets = nullptr;
int noOfLiveBullets;

void displayInfo()
{
    BN_LOG("Bullets: ", noOfBullets - curBullet, "\n\t\tPlayer HP: ", playerHP, "\n\t\tDealerHP: ", dealerHP);
}

void dealerPlays()
{
    if (noOfBullets - curBullet == 0)
    {
        return;
    }

    bn::fixed liveChance = bn::fixed(noOfLiveBullets) / (noOfBullets - curBullet);
    BN_LOG(noOfLiveBullets, " / (", noOfBullets, " - ", curBullet, ") = ", liveChance);
    bool dealersTurn = false;
    if (liveChance >= 0.5)
    {
        if (rrand.get_unbiased_int(10) == 0)
        {
            // shoot self
            if (bullets[curBullet])
            {
                dealerHP--;
                noOfLiveBullets--;
                BN_LOG("Dealer shot itself and hit (luck)");
            }
            else
            {
                BN_LOG("Dealer shot itself and missed (luck)");
            }
        }
        else
        {
            // shoot player
            if (bullets[curBullet])
            {
                playerHP--;
                noOfLiveBullets--;
                BN_LOG("Dealer shot player and hit");
            }
            else
            {
                BN_LOG("Dealer shot player and missed");
            }
        }
        curBullet++;
        displayInfo();
    }
    else
    {
        // shoot self
        if (bullets[curBullet])
        {
            dealerHP--;
            noOfLiveBullets--;
            BN_LOG("Dealer shot itself and hit");
        }
        else
        {
            BN_LOG("Dealer shot itself and missed");
            dealersTurn = true;
        }
        curBullet++;
        displayInfo();
    }

    if (dealersTurn)
    {
        dealerPlays();
    }
}

void resetChamber()
{
    curBullet = 0;
    noOfBullets = rrand.get_unbiased_int(2, 9);
    int lowlimit = noOfBullets > 4 ? 2 : 1;
    int uplimit = noOfBullets > 4 ? noOfBullets - 1 : noOfBullets;
    noOfLiveBullets = rrand.get_unbiased_int(lowlimit, uplimit);
    bullets = new bool[noOfBullets];

    for (int i = 0; i < noOfLiveBullets; i++)
    {
        bullets[i] = true;
    }

    // shuffle bullet array
    for (int i = 0; i < noOfBullets; i++)
    {
        int j = rrand.get_unbiased_int(noOfBullets);
        bool temp = bullets[i];
        bullets[i] = bullets[j];
        bullets[j] = temp;
    }

    for (int i = 0; i < noOfBullets; i++)
    {
        BN_LOG(bullets[i]);
    }

    BN_LOG("Live Bullets: ", noOfLiveBullets, "\n\t\tTotal Bullets: ", noOfBullets);
}

void game(bn::sprite_text_generator &text_generator)
{
    text_generator.set_center_alignment();

    resetChamber();

    bn::sprite_ptr dealer = bn::sprite_items::test_dealer.create_sprite(0, -30);
    bool gameOver = false;
    bool canInput = true;
    bool dealersTurn = false;
    int dealer_sin = 0;
    playerHP = 3;
    dealerHP = playerHP;
    
    while (!gameOver && canInput)
    {
        // Shoot Dealer
        if (bn::keypad::a_pressed())
        {
            if (bullets[curBullet])
            {
                dealerHP--;
                noOfLiveBullets--;
                BN_LOG("Player shot dealer and hit");
                // dealer plays
                dealersTurn = true;
            }
            else
            {
                // lose turn, dealer plays
                BN_LOG("Player shot dealer and missed");
                dealersTurn = true;
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
                noOfLiveBullets--;
                BN_LOG("Player shot itself and hit");
                // dealer plays
                dealersTurn = true;
            }
            else
            {
                BN_LOG("Player shot itself and missed");
            }
            curBullet++;
            displayInfo();
        }

        if (dealersTurn)
        {
            dealersTurn = false;
            dealerPlays();
        }

        if (curBullet == noOfBullets)
        {
            resetChamber();
        }

        if (playerHP <= 0 || dealerHP <= 0)
        {
            gameOver = true;
            displayInfo();
            BN_LOG("GAME OVER");
        }

        bn::fixed yPos = bn::degrees_sin(dealer_sin) * 5;
        dealer.set_position(bn::fixed_point(0, -30) + bn::fixed_point(0, yPos));
        dealer_sin += 3;
        bn::core::update();
    }
}