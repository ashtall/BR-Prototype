#include "bn_core.h"
#include "bn_random.h"
#include "bn_seed_random.h"
#include "bn_log.h"
#include "bn_keypad.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_time.h"
#include "bn_optional.h"
#include "bn_date.h"

#include "common_variable_8x16_sprite_font.h"

void game(bn::sprite_text_generator &text_generator)
{
    text_generator.set_center_alignment();
    bn::string<24> str = "";

    if (!bn::time::active())
    {
        BN_LOG(bn::time::active());
        str = "RTC is not enabled";
        bn::vector<bn::sprite_ptr, 32> text_sprites;
        text_generator.generate(0, 0, str, text_sprites);
        bn::core::update();
        while (true)
        {
        }
    }
    else
    {
        bn::seed_random rand;
        bn::optional<bn::time> time = bn::time::current();
        rand.set_seed(time->hour() * 10000 + time->minute() * 100 + time->second());

        int noOfBullets = 4;
        int lowlimit = noOfBullets > 4 ? 2 : 1;
        int uplimit = noOfBullets > 4 ? noOfBullets - 1 : noOfBullets;
        int noOfLiveBullets = rand.get_unbiased_int(lowlimit, uplimit);
        bool *bullets = new bool[noOfBullets];

        for(int i = 0;i<noOfLiveBullets;i++){
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

        for(int i = 0; i<noOfBullets;i++){
            BN_LOG(bullets[i]);
        }

        BN_LOG(noOfLiveBullets, noOfBullets);

        while (true)
        {
            if (bn::keypad::a_pressed())
            {
                int no = rand.get_unbiased_int(10);
                BN_LOG("A ", no);
                bn::string<16> diag_string = bn::to_string<16>(no);
                str = diag_string;
            }
            bn::vector<bn::sprite_ptr, 32> text_sprites;
            text_generator.generate(0, 0, str, text_sprites);
            bn::core::update();
        }
    }
}

int main()
{
    bn::core::init();

    bn::sprite_text_generator debug_text_gen(common::variable_8x16_sprite_font);

    while (true)
    {

        game(debug_text_gen);
        bn::core::update();
    }
}
