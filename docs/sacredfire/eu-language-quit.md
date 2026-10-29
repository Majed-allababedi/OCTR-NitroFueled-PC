# EU Language and localized Quit (CUSA14876 1.21)

The EU edition has six installed menu-text locales: Arabic (`ar`), German (`de`), English
(`en`), Spanish from Spain (`es`), French (`fr`) and Italian (`it`). Generic language names
in the authored Language Select page are not evidence of additional installed translations.

For EU, retain the game's Language Select page, event handling and every original Options
caption. Do not redirect its row to the USA Graphics menu. The Language control keeps its
original object name and cached name hash; only its existing hidden alpha/keyframes change.
Its native Hide exception must identify the exact owned Options control, not its position.
Privacy is not enabled or repurposed. USA retains its existing Graphics behavior.

Quit continues to use the existing main-menu control and intercepted quit-map request.
Its label and hint cover all six EU locales; these are port-authored strings, not claimed
original game translations. Arabic uses presentation forms in logical order, like the
original tables: `خروج` fits the existing label slot without growing or reindexing strings.
The generator fails closed rather than emitting a partially localized EU Quit overlay,
and the output version changes so older Graphics-labelled caches are not reused.

## Verification boundaries

Synthetic tests cover regional routing, localized string bounds and unchanged neighbours.
The optional `BB_TEST_GAME` test checks generated assets against a locally supplied edition;
no game files or generated assets belong in this repository. A private read-only EU archive
check can verify all six original Options tables remain byte-identical and all six Quit
labels/hints fit. These structural checks are not a claim that every locale was played or
that the new Arabic Quit text has been visually accepted in-game. An earlier isolated
experiment demonstrated the native Language page and a clean Quit, not this extracted
upstream candidate. No Privacy consent or saved-language changes are part of the tests.
