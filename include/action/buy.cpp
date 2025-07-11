#include "pch.hpp"
#include "store.hpp"
#include "on/SetBux.hpp"
#include "buy.hpp"

#include "tools/string.hpp"

void action::buy(ENetEvent& event, const std::string& header)
{
    std::vector<std::string> pipes = readch(std::move(header), '|');
    if (pipes.size() < 4) return;

    auto &peer = _peer[event.peer];

    short No = (peer->slot_size - 16) / 10 + 1; // @note number of upgrades | credits: https://growtopia.fandom.com/wiki/Backpack_Upgrade
    unsigned backpack_cost = (100 * No * No - 200 * No + 200);

    if (pipes[3] == "main") action::store(event, "");
    else if (pipes[3] == "upgrade_backpack") 
    {
        if (peer->gems < backpack_cost) 
        {
            packet::create(*event.peer, false, 0, 
            {
                "OnStorePurchaseResult",
                std::format(
                    "You can't afford `0Upgrade Backpack`` (`w10 Slots``)!  You're `${}`` Gems short.",
                    backpack_cost - peer->gems
                ).c_str()
            });
        }
        else 
        {
            packet::create(*event.peer, false, 0, 
            {
                "OnStorePurchaseResult",
                std::format(
                    "You've purchased `0Upgrade Backpack (10 Slots)`` for `${}`` Gems.\nYou have `${}`` Gems left.\n\n`5Received: ```0Backpack Upgrade``",
                    backpack_cost, peer->gems -= backpack_cost
                ).c_str()
            });
            on::SetBux(event);
            peer->slot_size += 10;
            inventory_visuals(event);
        }
    }
    else if (pipes[3] == "locks") 
    {
        packet::create(*event.peer, false, 0, 
        {
            "OnStoreRequest",
            std::format(
                "set_description_text|`2Locks And Stuff!``  Select the item you'd like more info on, or BACK to go back.\n"

                /* tabs */
                "enable_tabs|1\n"
                "add_tab_button|main_menu|Home|interface/large/btn_shop.rttex||0|0|0|0||||-1|-1|||0|0|CustomParams:|\n"
                "add_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop.rttex||1|1|0|0||||-1|-1|||0|0|CustomParams:|\n"
                "add_tab_button|itempack_menu|Item Packs|interface/large/btn_shop.rttex||0|3|0|0||||-1|-1|||0|0|CustomParams:|\n"
                "add_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop.rttex||0|4|0|0||||-1|-1|||0|0|CustomParams:|\n"
                "add_tab_button|weather_menu|Weather Machines|interface/large/btn_shop.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1|||0|0|CustomParams:|\n"
                "add_tab_button|token_menu|Growtoken Items|interface/large/btn_shop.rttex||0|2|0|0||||-1|-1|||0|0|CustomParams:|\n"
                
                /* restock items */
                "add_button|world_lock|`oWorld Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 World Lock.<CR><CR>`5Description:`` Become the undisputed ruler of your domain with one of these babies.  It works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.``  `wRecycles for 200 Gems.``<CR><CR>Available: 100/100``<CR>Restock in: `59 Hours 9 Mins``|0|7|2000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_custom_button|limiterTimeBG_world_lock|image:interface/large/gui_store_button_overlays11.rttex;image_size:256,160;frame:0,0;state:disabled;color:255,255,255,255;border:none;width:0.31;iconAlignment:CENTER;anchor:world_lock;display:none;action:isduhfiudfghids;top:0.5;left:0.5;|\n"
                "add_label|`w9 Hours``|target:world_lock;size:tiny;top:0.67;left:0.25|\n"
                "add_button|world_lock_10_pack|`oWorld Lock Pack``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 10 World Locks.<CR><CR>`5Description:`` 10-pack of World Locks. Become the undisputed ruler of up to TEN kingdoms with these babies. Each works like a normal lock except it locks the `$entire world``!  Won't work on worlds that other people already have locks on. You can even add additional normal locks to give access to certain areas to friends. `5It's a perma-item, is never lost when destroyed.`` `wEach recycles for 200 Gems.``<CR><CR>Available: 100/100``<CR>Restock in: `59 Hours 9 Mins``|0|3|20000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_custom_button|limiterTimeBG_world_lock_10_pack|image:interface/large/gui_store_button_overlays11.rttex;image_size:256,160;frame:0,0;state:disabled;color:255,255,255,255;border:none;width:0.31;iconAlignment:CENTER;anchor:world_lock_10_pack;display:none;action:isduhfiudfghids;top:0.5;left:0.5;|\n"
                "add_label|`w9 Hours``|target:world_lock_10_pack;size:tiny;top:0.67;left:0.25|\n"
                "add_button|anzu_pack1|`oAdvertisers Pack``|interface/large/store_buttons/store_buttons36.rttex|`2You Get:`` 1 Ad Control Block and a selection of Ad blocks. <CR><CR>`5Description:`` With this pack you can build custom Ad billboards in your world. There are 3 sizes you can choose from, 2x3, 3x2 and 6x1. You need to arrange these in a grid to activate the Ad. Remember to check your Gem earnings each day using the Ad Control Block.|0|1|10000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|small_lock|`oSmall Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Small Lock.<CR><CR>`5Description:`` Protect up to `$10`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``<CR><CR>Available: 100/100``<CR>Restock in: `59 Hours 9 Mins``|1|3|50|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_custom_button|limiterTimeBG_small_lock|image:interface/large/gui_store_button_overlays11.rttex;image_size:256,160;frame:0,0;state:disabled;color:255,255,255,255;border:none;width:0.31;iconAlignment:CENTER;anchor:small_lock;display:none;action:isduhfiudfghids;top:0.5;left:0.5;|\n"
                "add_label|`w9 Hours``|target:small_lock;size:tiny;top:0.67;left:0.25|\n"
                "add_button|big_lock|`oBig Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Big Lock.<CR><CR>`5Description:`` Protect up to `$48`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``<CR><CR>Available: 100/100``<CR>Restock in: `59 Hours 9 Mins``|1|1|200|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_custom_button|limiterTimeBG_big_lock|image:interface/large/gui_store_button_overlays11.rttex;image_size:256,160;frame:0,0;state:disabled;color:255,255,255,255;border:none;width:0.31;iconAlignment:CENTER;anchor:big_lock;display:none;action:isduhfiudfghids;top:0.5;left:0.5;|\n"
                "add_label|`w9 Hours``|target:big_lock;size:tiny;top:0.67;left:0.25|\n"
                "add_button|huge_lock|`oHuge Lock``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Huge Lock.<CR><CR>`5Description:`` Protect up to `$200`` tiles.  Can add friends to the lock so others can edit that area as well. `5It's a perma-item, is never lost when destroyed.``<CR><CR>Available: 100/100``<CR>Restock in: `59 Hours 9 Mins``|0|4|500|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_custom_button|limiterTimeBG_huge_lock|image:interface/large/gui_store_button_overlays11.rttex;image_size:256,160;frame:0,0;state:disabled;color:255,255,255,255;border:none;width:0.31;iconAlignment:CENTER;anchor:huge_lock;display:none;action:isduhfiudfghids;top:0.5;left:0.5;|\n"
                "add_label|`w9 Hours``|target:huge_lock;size:tiny;top:0.67;left:0.25|\n"
                
                "add_button|builders_lock|`oBuilder's Lock``|interface/large/store_buttons/store_buttons17.rttex|`2You Get:`` 1 Builders Lock.<CR><CR>`5Description:`` Protect up to `$200`` tiles. Wrench the lock to limit it - it can either only allow building, or only allow breaking! `5It's a perma-item, is never lost when destroyed.``|0|2|50000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|upgrade_backpack|`0Upgrade Backpack`` (`w10 Slots``)|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 10 Additional Backpack Slots.<CR><CR>`5Description:`` Sewing an extra pocket onto your backpack will allow you to store `$10`` additional item types.  How else are you going to fit all those toilets and doors?|0|1|{0}|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|ectojuicer|`oEctoJuicer``|interface/large/store_buttons/store_buttons20.rttex|`2You Get:`` 1 EctoJuicer.<CR><CR>`5Description:`` Infuse your muscles with the unearthly might of the Other Side! This spectral potion gives you the strength to wring every last drop of ectoplasm from a defeated Boss Ghost, granting you an EXTRA Boss Goo after a successful banishing!|0|0|30000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|grow_spray|`o5-pack of Grow Spray Fertilizer``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 5 Grow Spray Fertilizers.<CR><CR>`5Description:`` Why wait?!  Treat yourself to a `$5-pack`` of amazing `wGrow Spray Fertilizer`` by GrowTech Corp.  Each bottle instantly ages a tree by `$1 hour``.|0|6|200|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|deluxe_grow_spray|`oDeluxe Grow Spray``|interface/large/store_buttons/store_buttons11.rttex|`2You Get:`` 1 Deluxe Grow Spray.<CR><CR>`5Description:`` GrowTech's new `$Deluxe`` `wGrow Spray`` instantly ages a tree by `$24 hours`` per bottle! That's somewhere around 25 times as much as regular Grow Spray!|0|2|900|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|fish_flakes|`oFish Flakes``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 5 Fish Flakes.<CR><CR>`5Description:`` Every fish adores these tasty flakes! Give a pinch to your Training Fish and fill their scaly bellies with aquatic goodness! Take the guesswork out of finnicky feedings with a treat you know they'll love!|0|2|7500|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|fish_medicine|`oFish Medicine``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 1 Fish Medicine.<CR><CR>`5Description:`` Make a sick Training Fish bright and healthy with this healing potion. One dose is enough to make even the sickest fish all better!|0|0|1500|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|fish_reviver|`oFish Reviver``|interface/large/store_buttons/store_buttons18.rttex|`2You Get:`` 1 `#Rare Fish Reviver``.<CR><CR>`5Description:`` Resurrect a dead Training Fish with a revivifying zap from this `#Rare`` Fish Reviver! One dose is enough to reach beyond the veil and bring a fish back from the dead! Comes with a 100% zombie-free guarantee!|0|1|5000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|signal_jammer|`oSignal Jammer``|interface/large/store_buttons/store_buttons.rttex|`2You Get:`` 1 Signal Jammer.<CR><CR>`5Description:`` Get off the grid! Install a `$Signal Jammer``! A single punch will cause it to whir to life, tireless hiding your world and its population from pesky snoopers - only those who know the world name will be able to enter. `5It's a perma-item, is never lost when destroyed.``|1|6|2000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|zombie_jammer|`oZombie Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Zombie Jammer.<CR><CR>`5Description:`` Got a parkour or race that you don't want slowed down? Turn this on and nobody can be infected by zombie bites in your world. It does not prevent direct infection by the g-Virus itself though. `5It's a perma-item, is never lost when destroyed.``|0|5|15000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|punch_jammer|`oPunch Jammer``|interface/large/store_buttons/store_buttons7.rttex|`2You Get:`` 1 Punch Jammer.<CR><CR>`5Description:`` Tired of getting bashed around? Set up a Punch Jammer in your world, and people won't be able to punch each other! Can be turned on and off as needed. `5It's a perma-item, is never lost when destroyed.``|0|4|15000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|change_addr|`oChange of Address``|interface/large/store_buttons/store_buttons12.rttex|`2You Get:`` 1 Change of Address.<CR><CR>`5Description:`` Don't like the name of your world? You can use up one of these to trade your world's name with the name of any other world that you own. You must have a `2World Lock`` in both worlds. Go lock up that empty world with the new name you want and swap away!|0|6|20000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|door_mover|`oDoor Mover``|interface/large/store_buttons/store_buttons8.rttex|`2You Get:`` 1 Door Mover.<CR><CR>`5Description:`` Unsatisfied with your world's layout?  This one-use device can be used to move the White Door to any new location in your world, provided there are 2 empty spaces for it to fit in. Disappears when used. `2Only usable on a world you have World Locked.``|0|6|5000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|vending_machine|`oVending Machine``|interface/large/store_buttons/store_buttons13.rttex|`2You Get:`` 1 Vending Machine.<CR><CR>`5Description:`` Tired of interacting with human beings? Try a Vending Machine! You can put a stack of items inside it, set a price in World Locks, and people can buy from the machine while you sit back and rake in the profits! `5It's a perma-item, is never lost when destroyed, and it is not available any other way.``|0|6|8000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|digi_vend|`oDigiVend Machine``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 DigiVend Machine.<CR><CR>`5Description:`` Get with the times and go digital! This wired vending machine can connect its contents to Vending Hubs AND the multiversal economy, providing a unified shopping experience along with price checks to help you sell your goods! All that, and still no human-related hassle! Use your wrench on this to stock it with an item and set a price in World Locks. Other players will be able to buy from it! Only works in World-Locked worlds.|0|2|12000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                "add_button|checkout_counter|`oVending Hub - Checkout Counter``|interface/large/store_buttons/store_buttons29.rttex|`2You Get:`` 1 Vending Hub.<CR><CR>`5Description:`` Your one-stop shop! This vending hub will collect and display (and let shoppers buy) the contents of ALL DigiVends in its row or column (wrench it to set which the direction)! Wow! Now that's a shopping experience we can all enjoy! Note: Only works in World-Locked worlds.|0|3|50000|0|||-1|-1||-1|-1||1||||||0|0|CustomParams:|\n"
                ,backpack_cost
            ).c_str()
        });
    }
}