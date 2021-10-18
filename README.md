# copy-calling

What is this?
Some anticheats might try to IAT hook some functions to make sure it does not get used maliciously. This project will allocate some memory and itterate from the start adress to the function untill it finds 0xC3 (return). It will copy all of those bytes to the allocated memory and return that. Now whenever you call this function it wont trigger an IAT hook.

## Functionality
* Bypass some AntiCheats IAT hooks.

## Need help?
* Build the program in release x64 and with multibyte.
* Still cant figure it out? join the discord!

## Links
* [IAT Hooking](https://www.ired.team/offensive-security/code-injection-process-injection/import-adress-table-iat-hooking) - Iat Hooking
* [Discord](https://discord.gg/9XykzWqVMP) - GameReversalClub
