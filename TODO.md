## Pet Battles
1. Attacks like the cocoon one that protects the caster, should immediately become available.
2. Attacks like Elementium Bolt are missing their animations when they proc (multi-turn / delayed attacks)

## Pandaria
### Tillers
To Andrew, test everything and continue the plan in tmux

Thread 1 "worldserver" received signal SIGSEGV, Segmentation fault.
0x00005555561977b6 in ObjectGuid::ObjectGuid (this=0x7fffffff2ee0, r=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Entities/Object/ObjectGuid.h:167
167             ObjectGuid(ObjectGuid const& r) : _data(r._data) { }
(gdb) bt
#0  0x00005555561977b6 in ObjectGuid::ObjectGuid (this=0x7fffffff2ee0, r=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Entities/Object/ObjectGuid.h:167
#1  0x0000555557c79210 in Object::GetGuidValue (this=0x7ffeeba64640, index=0) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Entities/Object/Object.cpp:411
#2  0x0000555556197b85 in Object::GetGUID (this=0x7ffeeba64640) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Entities/Object/Object.h:89
#3  0x0000555558c27b71 in PlayerbotsMgr::GetPlayerbotAI (this=0x55555a6d8180 <PlayerbotsMgr::instance()::instance>, player=0x7ffeeba64640) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:1309
#4  0x0000555558c237bd in PlayerbotHolder::LogoutBotsWithoutRealPlayerInGroup (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:266
#5  0x0000555558c48caa in RandomPlayerbotMgr::OnPlayerLogout (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, player=0x7ffeea285a80) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/RandomPlayerbotMgr.cpp:815
#6  0x0000555558c08c1f in PlayerbotsScript::OnPlayerbotLogout (this=0x7fff2bfbd000, player=0x7ffeea285a80) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/mod_playerbots.cpp:168
#7  0x000055555832a3b4 in operator() (__closure=0x7fffffff31d0, script=0x7fff2bfbd000) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:55
#8  0x000055555832bfcc in std::__invoke_impl<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(std::__invoke_other, struct {...} &) (__f=...) at /usr/include/c++/13/bits/invoke.h:61
#9  0x000055555832b9a9 in std::__invoke_r<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(struct {...} &) (__fn=...) at /usr/include/c++/13/bits/invoke.h:111
#10 0x000055555832b053 in std::_Function_handler<void(PlayerbotScript*), ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)> >::_M_invoke(const std::_Any_data &, PlayerbotScript *&&) (__functor=..., __args#0=@0x7fffffff3120: 0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:290
#11 0x000055555832ac5f in std::function<void(PlayerbotScript*)>::operator() (this=0x7fffffff31d0, __args#0=0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:591
#12 0x000055555832a717 in ExecuteScript<PlayerbotScript> (executeHook=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/ScriptMgrMacros.h:68
#13 0x000055555832a403 in ScriptMgr::OnPlayerbotLogout (this=0x55555a4420a0 <ScriptMgr::instance()::instance>, player=0x7ffeea285a80) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:53
#14 0x000055555844f06b in WorldSession::LogoutPlayer (this=0x7ffeea032f00, save=true) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Server/WorldSession.cpp:540
#15 0x0000555558c24091 in PlayerbotHolder::LogoutPlayerBot (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, guid=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:426
#16 0x0000555558c23905 in PlayerbotHolder::LogoutBotsWithoutRealPlayerInGroup (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:291
#17 0x0000555558c48caa in RandomPlayerbotMgr::OnPlayerLogout (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, player=0x7ffee38fcec0) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/RandomPlayerbotMgr.cpp:815
#18 0x0000555558c08c1f in PlayerbotsScript::OnPlayerbotLogout (this=0x7fff2bfbd000, player=0x7ffee38fcec0) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/mod_playerbots.cpp:168
#19 0x000055555832a3b4 in operator() (__closure=0x7fffffff3740, script=0x7fff2bfbd000) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:55
#20 0x000055555832bfcc in std::__invoke_impl<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(std::__invoke_other, struct {...} &) (__f=...) at /usr/include/c++/13/bits/invoke.h:61
#21 0x000055555832b9a9 in std::__invoke_r<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(struct {...} &) (__fn=...) at /usr/include/c++/13/bits/invoke.h:111
#22 0x000055555832b053 in std::_Function_handler<void(PlayerbotScript*), ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)> >::_M_invoke(const std::_Any_data &, PlayerbotScript *&&) (__functor=..., __args#0=@0x7fffffff3690: 0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:290
#23 0x000055555832ac5f in std::function<void(PlayerbotScript*)>::operator() (this=0x7fffffff3740, __args#0=0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:591
#24 0x000055555832a717 in ExecuteScript<PlayerbotScript> (executeHook=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/ScriptMgrMacros.h:68
#25 0x000055555832a403 in ScriptMgr::OnPlayerbotLogout (this=0x55555a4420a0 <ScriptMgr::instance()::instance>, player=0x7ffee38fcec0) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:53
#26 0x000055555844f06b in WorldSession::LogoutPlayer (this=0x7ffee2404c00, save=true) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Server/WorldSession.cpp:540
#27 0x0000555558c24091 in PlayerbotHolder::LogoutPlayerBot (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, guid=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:426
#28 0x0000555558c23905 in PlayerbotHolder::LogoutBotsWithoutRealPlayerInGroup (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:291
#29 0x0000555558c48caa in RandomPlayerbotMgr::OnPlayerLogout (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, player=0x7ffee9c0b980) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/RandomPlayerbotMgr.cpp:815
#30 0x0000555558c08c1f in PlayerbotsScript::OnPlayerbotLogout (this=0x7fff2bfbd000, player=0x7ffee9c0b980) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/mod_playerbots.cpp:168
#31 0x000055555832a3b4 in operator() (__closure=0x7fffffff3cb0, script=0x7fff2bfbd000) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:55
#32 0x000055555832bfcc in std::__invoke_impl<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(std::__invoke_other, struct {...} &) (__f=...) at /usr/include/c++/13/bits/invoke.h:61
#33 0x000055555832b9a9 in std::__invoke_r<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(struct {...} &) (__fn=...) at /usr/include/c++/13/bits/invoke.h:111
#34 0x000055555832b053 in std::_Function_handler<void(PlayerbotScript*), ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)> >::_M_invoke(const std::_Any_data &, PlayerbotScript *&&) (__functor=..., __args#0=@0x7fffffff3c00: 0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:290
#35 0x000055555832ac5f in std::function<void(PlayerbotScript*)>::operator() (this=0x7fffffff3cb0, __args#0=0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:591
#36 0x000055555832a717 in ExecuteScript<PlayerbotScript> (executeHook=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/ScriptMgrMacros.h:68
#37 0x000055555832a403 in ScriptMgr::OnPlayerbotLogout (this=0x55555a4420a0 <ScriptMgr::instance()::instance>, player=0x7ffee9c0b980) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:53
#38 0x000055555844f06b in WorldSession::LogoutPlayer (this=0x7ffee8ddd700, save=true) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Server/WorldSession.cpp:540
#39 0x0000555558c24091 in PlayerbotHolder::LogoutPlayerBot (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, guid=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:426
#40 0x0000555558c23905 in PlayerbotHolder::LogoutBotsWithoutRealPlayerInGroup (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:291
#41 0x0000555558c48caa in RandomPlayerbotMgr::OnPlayerLogout (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, player=0x7ffee836c740) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/RandomPlayerbotMgr.cpp:815
#42 0x0000555558c08c1f in PlayerbotsScript::OnPlayerbotLogout (this=0x7fff2bfbd000, player=0x7ffee836c740) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/mod_playerbots.cpp:168
#43 0x000055555832a3b4 in operator() (__closure=0x7fffffff4220, script=0x7fff2bfbd000) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:55
#44 0x000055555832bfcc in std::__invoke_impl<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(std::__invoke_other, struct {...} &) (__f=...) at /usr/include/c++/13/bits/invoke.h:61
#45 0x000055555832b9a9 in std::__invoke_r<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(struct {...} &) (__fn=...) at /usr/include/c++/13/bits/invoke.h:111
#46 0x000055555832b053 in std::_Function_handler<void(PlayerbotScript*), ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)> >::_M_invoke(const std::_Any_data &, PlayerbotScript *&&) (__functor=..., __args#0=@0x7fffffff4170: 0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:290
#47 0x000055555832ac5f in std::function<void(PlayerbotScript*)>::operator() (this=0x7fffffff4220, __args#0=0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:591
#48 0x000055555832a717 in ExecuteScript<PlayerbotScript> (executeHook=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/ScriptMgrMacros.h:68
#49 0x000055555832a403 in ScriptMgr::OnPlayerbotLogout (this=0x55555a4420a0 <ScriptMgr::instance()::instance>, player=0x7ffee836c740) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:53
#50 0x000055555844f06b in WorldSession::LogoutPlayer (this=0x7ffee7970500, save=true) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Server/WorldSession.cpp:540
#51 0x0000555558c24091 in PlayerbotHolder::LogoutPlayerBot (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, guid=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:426
#52 0x0000555558c23905 in PlayerbotHolder::LogoutBotsWithoutRealPlayerInGroup (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:291
#53 0x0000555558c48caa in RandomPlayerbotMgr::OnPlayerLogout (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, player=0x7ffeea037ec0) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/RandomPlayerbotMgr.cpp:815
#54 0x0000555558c08c1f in PlayerbotsScript::OnPlayerbotLogout (this=0x7fff2bfbd000, player=0x7ffeea037ec0) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/mod_playerbots.cpp:168
#55 0x000055555832a3b4 in operator() (__closure=0x7fffffff4790, script=0x7fff2bfbd000) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:55
#56 0x000055555832bfcc in std::__invoke_impl<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(std::__invoke_other, struct {...} &) (__f=...) at /usr/include/c++/13/bits/invoke.h:61
#57 0x000055555832b9a9 in std::__invoke_r<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(struct {...} &) (__fn=...) at /usr/include/c++/13/bits/invoke.h:111
#58 0x000055555832b053 in std::_Function_handler<void(PlayerbotScript*), ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)> >::_M_invoke(const std::_Any_data &, PlayerbotScript *&&) (__functor=..., __args#0=@0x7fffffff46e0: 0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:290
#59 0x000055555832ac5f in std::function<void(PlayerbotScript*)>::operator() (this=0x7fffffff4790, __args#0=0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:591
#60 0x000055555832a717 in ExecuteScript<PlayerbotScript> (executeHook=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/ScriptMgrMacros.h:68
#61 0x000055555832a403 in ScriptMgr::OnPlayerbotLogout (this=0x55555a4420a0 <ScriptMgr::instance()::instance>, player=0x7ffeea037ec0) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:53
#62 0x000055555844f06b in WorldSession::LogoutPlayer (this=0x7ffee8dde100, save=true) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Server/WorldSession.cpp:540
#63 0x0000555558c24091 in PlayerbotHolder::LogoutPlayerBot (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, guid=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:426
#64 0x0000555558c23905 in PlayerbotHolder::LogoutBotsWithoutRealPlayerInGroup (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:291
#65 0x0000555558c48caa in RandomPlayerbotMgr::OnPlayerLogout (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, player=0x7ffee9a41740) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/RandomPlayerbotMgr.cpp:815
#66 0x0000555558c08c1f in PlayerbotsScript::OnPlayerbotLogout (this=0x7fff2bfbd000, player=0x7ffee9a41740) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/mod_playerbots.cpp:168
#67 0x000055555832a3b4 in operator() (__closure=0x7fffffff4d00, script=0x7fff2bfbd000) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:55
#68 0x000055555832bfcc in std::__invoke_impl<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(std::__invoke_other, struct {...} &) (__f=...) at /usr/include/c++/13/bits/invoke.h:61
#69 0x000055555832b9a9 in std::__invoke_r<void, ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)>&, PlayerbotScript*>(struct {...} &) (__fn=...) at /usr/include/c++/13/bits/invoke.h:111
#70 0x000055555832b053 in std::_Function_handler<void(PlayerbotScript*), ScriptMgr::OnPlayerbotLogout(Player*)::<lambda(PlayerbotScript*)> >::_M_invoke(const std::_Any_data &, PlayerbotScript *&&) (__functor=..., __args#0=@0x7fffffff4c50: 0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:290
#71 0x000055555832ac5f in std::function<void(PlayerbotScript*)>::operator() (this=0x7fffffff4d00, __args#0=0x7fff2bfbd000) at /usr/include/c++/13/bits/std_function.h:591
#72 0x000055555832a717 in ExecuteScript<PlayerbotScript> (executeHook=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/ScriptMgrMacros.h:68
#73 0x000055555832a403 in ScriptMgr::OnPlayerbotLogout (this=0x55555a4420a0 <ScriptMgr::instance()::instance>, player=0x7ffee9a41740) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Scripting/PlayerbotScript.cpp:53
#74 0x000055555844f06b in WorldSession::LogoutPlayer (this=0x7ffee8ddd200, save=true) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/src/server/game/Server/WorldSession.cpp:540
#75 0x0000555558c24091 in PlayerbotHolder::LogoutPlayerBot (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, guid=...) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:426
#76 0x0000555558c23905 in PlayerbotHolder::LogoutBotsWithoutRealPlayerInGroup (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/PlayerbotMgr.cpp:291
#77 0x0000555558c48caa in RandomPlayerbotMgr::OnPlayerLogout (this=0x55555a6d8200 <RandomPlayerbotMgr::instance()::instance>, player=0x7ffeea860980) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/Manager/RandomPlayerbotMgr.cpp:815
#78 0x0000555558c08c1f in PlayerbotsScript::OnPlayerbotLogout (this=0x7fff2bfbd000, player=0x7ffeea860980) at /home/andrew/Legends-of-Azeroth-Pandaria-5.4.8/modules/mod_playerbots/src/mod_playerbots.cpp:168
