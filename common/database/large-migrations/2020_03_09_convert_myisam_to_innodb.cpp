std::string _2020_03_09_convert_myisam_to_innodb = R"(
ALTER TABLE `account_flags` ENGINE=InnoDB;
ALTER TABLE `account_ip` ENGINE=InnoDB;
ALTER TABLE `account` ENGINE=InnoDB;
ALTER TABLE `adventure_template_entry_flavor` ENGINE=InnoDB;
ALTER TABLE `adventure_template_entry` ENGINE=InnoDB;
ALTER TABLE `altadv_vars` ENGINE=InnoDB;
ALTER TABLE `alternate_currency` ENGINE=InnoDB;
ALTER TABLE `banned_ips` ENGINE=InnoDB;
ALTER TABLE `base_data` ENGINE=InnoDB;
ALTER TABLE `blocked_spells` ENGINE=InnoDB;
ALTER TABLE `buyer` ENGINE=InnoDB;
ALTER TABLE `char_create_combinations` ENGINE=InnoDB;
ALTER TABLE `char_create_point_allocations` ENGINE=InnoDB;
ALTER TABLE `character_activities` ENGINE=InnoDB;
ALTER TABLE `character_enabledtasks` ENGINE=InnoDB;
ALTER TABLE `character_tasks` ENGINE=InnoDB;
ALTER TABLE `chatchannels` ENGINE=InnoDB;
ALTER TABLE `completed_tasks` ENGINE=InnoDB;
ALTER TABLE `damageshieldtypes` ENGINE=InnoDB;
ALTER TABLE `discovered_items` ENGINE=InnoDB;
ALTER TABLE `eqtime` ENGINE=InnoDB;
ALTER TABLE `eventlog` ENGINE=InnoDB;
ALTER TABLE `faction_list_mod` ENGINE=InnoDB;
ALTER TABLE `faction_list` ENGINE=InnoDB;
ALTER TABLE `faction_values` ENGINE=InnoDB;
ALTER TABLE `friends` ENGINE=InnoDB;
ALTER TABLE `goallists` ENGINE=InnoDB;
ALTER TABLE `guild_bank` ENGINE=InnoDB;
ALTER TABLE `guild_members` ENGINE=InnoDB;
ALTER TABLE `guild_ranks` ENGINE=InnoDB;
ALTER TABLE `guild_relations` ENGINE=InnoDB;
ALTER TABLE `guilds` ENGINE=InnoDB;
ALTER TABLE `hackers` ENGINE=InnoDB;
ALTER TABLE `horses` ENGINE=InnoDB;
ALTER TABLE `inventory_versions` ENGINE=InnoDB;
ALTER TABLE `item_tick` ENGINE=InnoDB;
ALTER TABLE `items` ENGINE=InnoDB;
ALTER TABLE `keyring` ENGINE=InnoDB;
ALTER TABLE `launcher_zones` ENGINE=InnoDB;
ALTER TABLE `launcher` ENGINE=InnoDB;
ALTER TABLE `ldon_trap_entries` ENGINE=InnoDB;
ALTER TABLE `ldon_trap_templates` ENGINE=InnoDB;
ALTER TABLE `lfguild` ENGINE=InnoDB;
ALTER TABLE `lootdrop_entries` ENGINE=InnoDB;
ALTER TABLE `lootdrop` ENGINE=InnoDB;
ALTER TABLE `loottable_entries` ENGINE=InnoDB;
ALTER TABLE `loottable` ENGINE=InnoDB;
ALTER TABLE `mail` ENGINE=InnoDB;
ALTER TABLE `merc_armorinfo` ENGINE=InnoDB;
ALTER TABLE `merc_buffs` ENGINE=InnoDB;
ALTER TABLE `merc_inventory` ENGINE=InnoDB;
ALTER TABLE `merc_merchant_entries` ENGINE=InnoDB;
ALTER TABLE `merc_merchant_template_entries` ENGINE=InnoDB;
ALTER TABLE `merc_merchant_templates` ENGINE=InnoDB;
ALTER TABLE `merc_name_types` ENGINE=InnoDB;
ALTER TABLE `merc_npc_types` ENGINE=InnoDB;
ALTER TABLE `merc_spell_list_entries` ENGINE=InnoDB;
ALTER TABLE `merc_spell_lists` ENGINE=InnoDB;
ALTER TABLE `merc_stance_entries` ENGINE=InnoDB;
ALTER TABLE `merc_stats` ENGINE=InnoDB;
ALTER TABLE `merc_subtypes` ENGINE=InnoDB;
ALTER TABLE `merc_templates` ENGINE=InnoDB;
ALTER TABLE `merc_types` ENGINE=InnoDB;
ALTER TABLE `merc_weaponinfo` ENGINE=InnoDB;
ALTER TABLE `mercs` ENGINE=InnoDB;
ALTER TABLE `name_filter` ENGINE=InnoDB;
ALTER TABLE `npc_types` ENGINE=InnoDB;
ALTER TABLE `object_contents` ENGINE=InnoDB;
ALTER TABLE `petitions` ENGINE=InnoDB;
ALTER TABLE `pets_equipmentset_entries` ENGINE=InnoDB;
ALTER TABLE `pets_equipmentset` ENGINE=InnoDB;
ALTER TABLE `player_titlesets` ENGINE=InnoDB;
ALTER TABLE `proximities` ENGINE=InnoDB;
ALTER TABLE `races` ENGINE=InnoDB;
ALTER TABLE `raid_details` ENGINE=InnoDB;
ALTER TABLE `raid_leaders` ENGINE=InnoDB;
ALTER TABLE `raid_members` ENGINE=InnoDB;
ALTER TABLE `rule_sets` ENGINE=InnoDB;
ALTER TABLE `rule_values` ENGINE=InnoDB;
ALTER TABLE `saylink` ENGINE=InnoDB;
ALTER TABLE `sharedbank` ENGINE=InnoDB;
ALTER TABLE `skill_caps` ENGINE=InnoDB;
ALTER TABLE `spell_globals` ENGINE=InnoDB;
ALTER TABLE `spells_new` ENGINE=InnoDB;
ALTER TABLE `task_activities` ENGINE=InnoDB;
ALTER TABLE `tasks` ENGINE=InnoDB;
ALTER TABLE `tasksets` ENGINE=InnoDB;
ALTER TABLE `timers` ENGINE=InnoDB;
ALTER TABLE `titles` ENGINE=InnoDB;
ALTER TABLE `trader_audit` ENGINE=InnoDB;
ALTER TABLE `trader` ENGINE=InnoDB;
ALTER TABLE `tradeskill_recipe_entries` ENGINE=InnoDB;
ALTER TABLE `tradeskill_recipe` ENGINE=InnoDB;
ALTER TABLE `variables` ENGINE=InnoDB;
ALTER TABLE `veteran_reward_templates` ENGINE=InnoDB;
)";