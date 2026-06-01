## Pet Battles
1. Attacks like the cocoon one that protects the caster, should immediately become available.
2. Attacks like Elementium Bolt are missing their animations when they proc (multi-turn / delayed attacks)

## Playerbots
1. Prevent lag spike when playerbots are logging in. Log a few in at a time to allow the server to remain responsive.
   - [x] Added `AiPlayerbot.MaxConcurrentBotLogins = 2` config (default 2 bot logins per tick)
   - [x] Capped login loop in `RandomPlayerbotMgr::UpdateAIInternal` to respect cap
   - [ ] Test on live server — verify no lag spike on login from zero players

## Pandaria
### Tillers
See TILLERS-TODO.md
