"""EU Quit text must cover the six localized resource tables."""
import unittest

from paths import ROOT  # adds scripts to the import path
import graphics_tab


class EuQuitLocaleTests(unittest.TestCase):
    def test_six_eu_resource_locales_have_localized_quit_text(self):
        locales = ('ar', 'de', 'en', 'es', 'fr', 'it')
        for locale in locales:
            with self.subTest(locale=locale):
                label, hint = graphics_tab.QUIT_LABELS[locale]
                self.assertTrue(label.strip())
                self.assertTrue(hint.strip())
                label.encode('utf-8', 'strict')
                hint.encode('utf-8', 'strict')
                if locale != 'en':
                    self.assertNotEqual((label, hint), graphics_tab.QUIT_LABELS['en'])


class EuLanguageTests(unittest.TestCase):
    def test_edition_selection_is_title_and_version_specific(self):
        from pathlib import Path
        import tempfile
        from test_game_check import param_sfo
        with tempfile.TemporaryDirectory() as tmp:
            game = Path(tmp)
            (game / 'sce_sys').mkdir()
            sfo = game / 'sce_sys/param.sfo'
            for title, expected in (('CUSA14876', 'EU'), ('CUSA13795', 'USA')):
                sfo.write_bytes(param_sfo({'TITLE_ID': title, 'APP_VER': '01.21'}))
                self.assertEqual(graphics_tab.menu_edition(game), expected)
            for title, version in (('CUSA14876', '01.00'), ('CUSA99999', '01.21')):
                sfo.write_bytes(param_sfo({'TITLE_ID': title, 'APP_VER': version}))
                with self.assertRaises(ValueError):
                    graphics_tab.menu_edition(game)
            sfo.unlink()
            with self.assertRaises(OSError):
                graphics_tab.menu_edition(game)

    def test_arabic_matches_shaped_logical_order_and_fits(self):
        import unicodedata
        label, hint = graphics_tab.QUIT_LABELS['ar']
        self.assertEqual(unicodedata.normalize('NFKC', label), 'خروج')
        self.assertEqual(unicodedata.normalize('NFKC', hint), 'إغلاق اللعبة والعودة إلى سطح المكتب.')
        self.assertTrue(any('\ufe70' <= c <= '\ufeff' for c in label))
        self.assertLessEqual(len(label.encode()) + 1, 16)

    def test_all_original_eu_captions_and_language_route_are_preserved(self):
        for locale in ('ar', 'de', 'en', 'es', 'fr', 'it'):
            original = ('native caption ' + locale).encode() + b'\0' + graphics_tab.ANCHOR + b'\0'
            self.assertEqual(graphics_tab.EU_CHANGES[f'ui/octane_options_menu_{locale}.lng'](original), original)
        self.assertIs(graphics_tab.EU_CHANGES['ui/octane_options_menu.igz'], graphics_tab.show_tab)
        self.assertNotIn('vsc/ui/octane_options_menu/octane_options_menu_languageselectbutton.igz', graphics_tab.EU_CHANGES)

    def test_localized_quit_keeps_offsets_and_neighbouring_strings(self):
        for locale in ('ar', 'de', 'en', 'es', 'fr', 'it'):
            label, hint = graphics_tab.QUIT_LABELS[locale]
            original = b'prefix\0' + b'h' * 191 + b'\0' + graphics_tab.QUIT_KEY + b'l' * 15 + b'\0tail\0'
            result = graphics_tab.quit_labels(label, hint)(original)
            self.assertEqual(len(result), len(original))
            self.assertEqual(result.index(graphics_tab.QUIT_KEY), original.index(graphics_tab.QUIT_KEY))
            self.assertTrue(result.endswith(b'\0tail\0'))
            self.assertIn(label.encode(), result)
            self.assertIn(hint.encode(), result)

    def test_incomplete_eu_quit_never_writes_partial_overlay(self):
        import contextlib
        import io
        from pathlib import Path
        from types import SimpleNamespace
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            game = root / 'game'
            (game / 'archives').mkdir(parents=True)
            names = [f'ui/octane_options_menu_{locale}.lng' for locale in ('ar', 'de', 'en', 'es', 'fr', 'it')]
            names += ['ui/octane_options_menu.igz', 'vsc/ui/octane_options_menu/octane_options_menu_root.igz']
            graphics_tab.write_archive(game / 'archives/update2.pak',
                [(name, name, 0, b'synthetic') for name in names], SimpleNamespace(flags=0))
            dest = root / 'out/update3.pak'
            with contextlib.redirect_stdout(io.StringIO()):
                with self.assertRaisesRegex(ValueError, 'Quit|quit|mainmenu'):
                    graphics_tab.build(game, dest, root / 'dump', edition='EU')
            self.assertFalse(dest.exists())


if __name__ == '__main__':
    unittest.main()
