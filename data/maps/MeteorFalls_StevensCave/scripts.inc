MeteorFalls_StevensCave_MapScripts::
	.byte 0

MeteorFalls_StevensCave_EventScript_Steven::
	lock
	goto_if_set FLAG_DEFEATED_METEOR_FALLS_STEVEN, MeteorFalls_StevensCave_EventScript_Defeated
	waitse
	playse SE_PIN
	applymovement LOCALID_METEOR_FALLS_STEVEN, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_METEOR_FALLS_STEVEN, Common_Movement_Delay48
	waitmovement 0
	applymovement LOCALID_METEOR_FALLS_STEVEN, Common_Movement_FacePlayer
	waitmovement 0
	msgbox MeteorFalls_StevensCave_Text_ShouldKnowHowGoodIAmExpectWorst, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_STEVEN, MeteorFalls_StevensCave_Text_StevenDefeat
	msgbox MeteorFalls_StevensCave_Text_MyPredictionCameTrue, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_METEOR_FALLS_STEVEN
	release
	end

MeteorFalls_StevensCave_EventScript_Defeated::
	applymovement LOCALID_METEOR_FALLS_STEVEN, Common_Movement_FacePlayer
	waitmovement 0
	goto_if_set FLAG_RECEIVED_STEVEN_TRI_PASS, MeteorFalls_StevensCave_EventScript_StevenPostTrade
	msgbox MeteorFalls_StevensCave_Text_StevenStarving, MSGBOX_DEFAULT
	checkitem ITEM_CHERI_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_CHESTO_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_PECHA_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_RAWST_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_ASPEAR_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_LEPPA_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_ORAN_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_PERSIM_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_LUM_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_SITRUS_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_FIGY_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_WIKI_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_MAGO_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_AGUAV_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_IAPAPA_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_RAZZ_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_BLUK_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_NANAB_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_WEPEAR_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_PINAP_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_POMEG_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_KELPSY_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_QUALOT_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_HONDEW_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_GREPA_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_TAMATO_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_CORNN_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_MAGOST_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_RABUTA_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_NOMEL_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_SPELON_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_PAMTRE_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_WATMEL_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_DURIN_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_BELUE_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_CHILAN_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	checkitem ITEM_ENIGMA_BERRY, 1
	goto_if_eq VAR_RESULT, FALSE, MeteorFalls_StevensCave_EventScript_MissingBerries
	msgbox MeteorFalls_StevensCave_Text_YouHaveThemAll, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MeteorFalls_StevensCave_EventScript_DeclinedTrade
	removeitem ITEM_CHERI_BERRY, 1
	removeitem ITEM_CHESTO_BERRY, 1
	removeitem ITEM_PECHA_BERRY, 1
	removeitem ITEM_RAWST_BERRY, 1
	removeitem ITEM_ASPEAR_BERRY, 1
	removeitem ITEM_LEPPA_BERRY, 1
	removeitem ITEM_ORAN_BERRY, 1
	removeitem ITEM_PERSIM_BERRY, 1
	removeitem ITEM_LUM_BERRY, 1
	removeitem ITEM_SITRUS_BERRY, 1
	removeitem ITEM_FIGY_BERRY, 1
	removeitem ITEM_WIKI_BERRY, 1
	removeitem ITEM_MAGO_BERRY, 1
	removeitem ITEM_AGUAV_BERRY, 1
	removeitem ITEM_IAPAPA_BERRY, 1
	removeitem ITEM_RAZZ_BERRY, 1
	removeitem ITEM_BLUK_BERRY, 1
	removeitem ITEM_NANAB_BERRY, 1
	removeitem ITEM_WEPEAR_BERRY, 1
	removeitem ITEM_PINAP_BERRY, 1
	removeitem ITEM_POMEG_BERRY, 1
	removeitem ITEM_KELPSY_BERRY, 1
	removeitem ITEM_QUALOT_BERRY, 1
	removeitem ITEM_HONDEW_BERRY, 1
	removeitem ITEM_GREPA_BERRY, 1
	removeitem ITEM_TAMATO_BERRY, 1
	removeitem ITEM_CORNN_BERRY, 1
	removeitem ITEM_MAGOST_BERRY, 1
	removeitem ITEM_RABUTA_BERRY, 1
	removeitem ITEM_NOMEL_BERRY, 1
	removeitem ITEM_SPELON_BERRY, 1
	removeitem ITEM_PAMTRE_BERRY, 1
	removeitem ITEM_WATMEL_BERRY, 1
	removeitem ITEM_DURIN_BERRY, 1
	removeitem ITEM_BELUE_BERRY, 1
	removeitem ITEM_CHILAN_BERRY, 1
	removeitem ITEM_ENIGMA_BERRY, 1
	msgbox MeteorFalls_StevensCave_Text_StevenThanksFeast, MSGBOX_DEFAULT
	giveitem ITEM_TRI_PASS
	setflag FLAG_RECEIVED_STEVEN_TRI_PASS
	msgbox MeteorFalls_StevensCave_Text_StevenGivesTriPass, MSGBOX_DEFAULT
	release
	end

MeteorFalls_StevensCave_EventScript_MissingBerries::
	msgbox MeteorFalls_StevensCave_Text_MissingBerries, MSGBOX_DEFAULT
	release
	end

MeteorFalls_StevensCave_EventScript_DeclinedTrade::
	msgbox MeteorFalls_StevensCave_Text_DeclinedTrade, MSGBOX_DEFAULT
	release
	end

MeteorFalls_StevensCave_EventScript_StevenPostTrade::
	msgbox MeteorFalls_StevensCave_Text_MyPredictionCameTrue, MSGBOX_DEFAULT
	release
	end

MeteorFalls_StevensCave_Text_ShouldKnowHowGoodIAmExpectWorst:
	.string "STEVEN: Oh, wow, {PLAYER}{KUN}.\n"
	.string "I'm amazed you knew where to find me.\p"
	.string "Do you, uh…maybe think of me as\n"
	.string "just a rock maniac?\p"
	.string "No, that can't be right.\p"
	.string "We battled alongside each other at\n"
	.string "the SOOTOPOLIS SPACE CENTER.\p"
	.string "You should have a very good idea\n"
	.string "about how good I am.\p"
	.string "Okay, {PLAYER}{KUN}, if you're going to mount\n"
	.string "a serious challenge, expect the worst!$"

MeteorFalls_StevensCave_Text_StevenDefeat:
	.string "You…\n"
	.string "I had no idea you'd become so strong…$"

MeteorFalls_StevensCave_Text_MyPredictionCameTrue:
	.string "STEVEN: Come to think of it, ever since\n"
	.string "our paths first crossed in GRANITE\l"
	.string "CAVE in DEWFORD, I had this feeling.\p"
	.string "I thought that you would eventually\n"
	.string "become the CHAMPION.\p"
	.string "My predictions usually come true.\p"
	.string "And where will you go from here?\p"
	.string "… … … … … …\n"
	.string "… … … … … …\p"
	.string "Fufufu, even I couldn't tell you that.$"


MeteorFalls_StevensCave_Text_StevenStarving:
	.string "STEVEN: Ohh… {PLAYER}{KUN}, you found me\n"
	.string "just in time.\p"
	.string "I've been hunched over these rocks for\n"
	.string "days and I am absolutely STARVING.\p"
	.string "What I wouldn't give for an enormous\n"
	.string "fruit salad… one of every BERRY sold by\l"
	.string "that vendor outside LILYCOVE\l"
	.string "DEPARTMENT STORE.\p"
	.string "Every single flavor, all in one bowl.\n"
	.string "Now THAT would be a meal fit for a\l"
	.string "former CHAMPION.$"

MeteorFalls_StevensCave_Text_MissingBerries:
	.string "STEVEN: A single BERRY missing would\n"
	.string "ruin the whole salad.\p"
	.string "Come back when you've gathered every\n"
	.string "last one from that LILYCOVE vendor.$"

MeteorFalls_StevensCave_Text_YouHaveThemAll:
	.string "STEVEN: Wait…\p"
	.string "You have EVERY single one?\p"
	.string "{PLAYER}{KUN}, would you… would you really\n"
	.string "part with all of them for my sake?$"

MeteorFalls_StevensCave_Text_DeclinedTrade:
	.string "STEVEN: Ha… of course. A trainer's\n"
	.string "BERRIES are precious.\p"
	.string "Don't mind me. I'll manage.$"

MeteorFalls_StevensCave_Text_StevenThanksFeast:
	.string "STEVEN: You're a lifesaver, {PLAYER}{KUN}!\p"
	.string "Oh, this salad is going to be\n"
	.string "MAGNIFICENT.\p"
	.string "Let me repay you properly…$"

MeteorFalls_StevensCave_Text_StevenGivesTriPass:
	.string "STEVEN: That TRI-PASS will let you ride\n"
	.string "the ferry between the three great\l"
	.string "ports of the region.\p"
	.string "Consider it a gift from one former\n"
	.string "CHAMPION to the current one.\p"
	.string "Now, if you'll excuse me… I have some\n"
	.string "SERIOUS eating to do.$"
