((c-mode . ((mode . c++)))
 (c++-mode . ((eval . (setq flycheck-gcc-include-path
                            '("include" "/usr/include/python3.5m")))
              (flycheck-gcc-language-standard . "gnu++17"))))
