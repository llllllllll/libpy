((c-mode . ((mode . c++)
            (eval . (c-set-offset 'innamespace 0))))
 (c++-mode . ((eval . (setq flycheck-gcc-include-path
                            '("include" "/usr/include/python3.5m")))
              (eval . (c-set-offset 'innamespace 0))
              (flycheck-gcc-language-standard . "gnu++17"))))
