"""
Find more penguins here (and credit to artists):

https://ascii.co.uk/art/penguins
http://www.asciiworld.com/-Penguin-.html


Current ASCII art includes:
    Formal penguin:     formal()
    Detailed penguin:   detailed()
    Ubuntu Penguin:     ubuntu()
    Simple Penguin:     simple()
    axOS:               axOS()
    Amber Lab:          amberlab()
"""

import time

class Penguins:
    @classmethod
    def formal(cls):
        a = """
                     xxxxx
                  xXXXXXXXXXx
                 XXXXXXXXXXXXX
                xXXXXXXXX  XXXx
                XXXXXXXXX 0XXXX\\\\\\
               xXXXXXXXXXxxXXXX\\\\\\\\
               XXXXXXXXXXXXXXXX//////
               XXXXXXXXXXXXXXXXX
               XXXXX|\XXX/|XXXXX
               XXXXX| \-/ |XXXXX
              xXXXXX| [ ] |XXXXXx
            xXXXX   | /-\ |   XXXXx
         xXXXXX     |/   \|     XXXXXx
       xXXXXXX                   XXXXXXx
      xXXXXXXX                   XXXXXXXx
     xXXXXXXXX                   XXXXXXXXx
    xXXXXXXXXX                   XXXXXXXXXx
   xXXXXXXXXXX                   XXXXXXXXXXx
  xXXXXXXXXXXX                   XXXXXXXXXXXx
 xXXXXXXXX XXX                   XXX XXXXXXXXx
 XXXXXXXX  XXX                   XXX  XXXXXXXX
xXXXXXXX   XXX                   XXX   XXXXXXXx
XXXXXX     XXX                   XXX     XXXXXX
XXXX       XXX                   XXX       XXXX
 XX        XXX                   XXX        XX
           XXX                   XXX
           XXX                   XXX
           XXX                   XXX
           XXX                   XXX
           XXXx                 xXXX    
           XXXXXXXXXXXXXXXXXXXXXXXXX
           XXXXXXX           XXXXXXX
       ____XXXXXX             XXXXXX____
      /________/               \________\
        """
        Penguins.printing(a, 0.0005)

    @classmethod
    def detailed(cls):
        a = """
                            ooo
                               ooo$$$$$$$$$$$oo
                            o$$$$$$$$$$$$$$$$$$$ooo
                          o$$$$$$$$$$$$$$$$$"$$$$$$$oo
                       o$$$$$$$$$$$$$$$$$$$  o $$$$$$$$$$$$$$oooo
                      o$$$$$$$$$$$$$$$$$$$$oooo$$$$$$$$$$$$$$$$
                    oo$""      "$$$$$$$$$$$$$$$$$$$$$$$$"
                   o$$          $$$$$$$$$$$$$$$$$$$$$$"
                  o$$            $$$$$$$$$$$$$$$$$$$$
                o$$$$             $$$$$$$$$$$$$$$$$"
               o$$$$$$oooooooo "                $"
              $$$$$$$$$$$$$$"
             $$$$$$$$$$$$$$
            o$$$$$$$$$$$$$                         o
           o$$$$$$$$$$$$$                           o
          o$$$$$$$$$$$$$$                            "o
         o$$$$$$$$$$$$$$$                             "o
        o$$$$$$$$$$$$$$$$                              $
       o$$$$$$$$$$$$$$$$"                              "
       $$$$$$$$$$$$$$$$$                                $
      o$$$$$$$$$$$$$$$$$                                $
      $$$$$$$$$$$$$$$$$                                 $
     $$$$$$$$$$$$$$$$$$                                 "
     $$$$$$$$$$$$$$$$$                                   $
    $$$$$$$$$$$$$$$$$                                    $
    $$$$$$$$$$$$$$$$"                                    $$
   $$$$$$$$$$$$$$$$$                                      $o
   $$$$$$$$$$$$$$$$$                                      $$
  $$$$$$$$$$$$$$$$$$                                       $
  $$$$$$$$$$$$$$$$$$o                                      $$
 $$$$$$$$$$$$$$$$$$$$o                                     $$
 $$$$$$$$$$$$$$$$$$$$$$o                                   "$
 $$$$$$$$$$$$$$$$$$$$$$$$o                                  $
$$$$$$$$$$$$$$$$$$$$$$$$$$$                                 $$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$                                $$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$                               $$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o                              $$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o                             $$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$                             $$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$                             $"
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$                            $$
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$                            $"
$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$                            $
 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$                            o$
 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$                            $"
 $$$$$$$$$$$$$$$$$$$$$$$$$$$$"                            $
 $$$$$$$$$$$$$$$$$$$$$$$$$$$"                             $
  $$$$$$$$$$$$$$$$$$$$$$$$$"                             $$
  $$$$$$$$$$$$$$$$$$$$$$$$"                              $$
  $$$$$$$$$$$$$$$$$$$$$$$                                $$
   $$$$$$$$$$$$$$$$$$$$$                                o$$
   $$$$$$$$$$$$$$$$$$$$                                 $$"
   "$$$$$$$$$$$$$$$$$$                                  $$
    $$$$$$$$$$$$$$$$$                                  o$$
    "$$$$$$$$$$$$$$$"                                  $$
     $$$$$$$$$$$$$$$                                   $$
     "$$$$$$$$$$$$$"                                  o$
      $$$$$$$$$$$$"                                   $$
      $$$$$$$$$$$"                                    $$
       $$$$$$$$$"                                    $$"
       $$$$$$$$$                                     $$
       "$$$$$$$"                                    $$
        $$$$$$$o                                    $"
       o$$$$$$$$                                   $$
       $$$$$$$$$                                   $$
      o$$$$$$$$$                                   $"
      $$$$$$$$$$                                  $$
      "$$$$$$$$$                                o$""
       "$$$$$$$$                          ooooo$$oo
          ""$$$$$o                oooo$$$$$$$$$$$$$$ooo
             "$$$$$$ooooooo     $$$$$$$$$$$$$$$$$$o   $$
                                                  "
        """
        Penguins.printing(a, 0.0001)

    @classmethod
    def ubuntu(cls):
        a = """
                                 .:xxxxxxxx:.
                             .xxxxxxxxxxxxxxxx.
                            :xxxxxxxxxxxxxxxxxxx:.
                           .xxxxxxxxxxxxxxxxxxxxxxx:
                          :xxxxxxxxxxxxxxxxxxxxxxxxx:
                          xxxxxxxxxxxxxxxxxxxxxxxxxxX:
                          xxx:::xxxxxxxx::::xxxxxxxxx:
                         .xx:   ::xxxxx:     :xxxxxxxx
                         :xx  x.  xxxx:  xx.  xxxxxxxx
                         :xx xxx  xxxx: xxxx  :xxxxxxx
                         'xx 'xx  xxxx:. xx'  xxxxxxxx
                          xx ::::::xx:::::.   xxxxxxxx
                          xx:::::.::::.:::::::xxxxxxxx
                          :x'::::'::::':::::':xxxxxxxxx.
                          :xx.::::::::::::'   xxxxxxxxxx
                          :xx: '::::::::'     :xxxxxxxxxx.
                         .xx     '::::'        'xxxxxxxxxx.
                       .xxxx                     'xxxxxxxxx.
                     .xxxx                         'xxxxxxxxx.
                   .xxxxx:                          xxxxxxxxxx.
                  .xxxxx:'                          xxxxxxxxxxx.
                 .xxxxxx:::.           .       ..:::_xxxxxxxxxxx:.
                .xxxxxxx''      ':::''            ''::xxxxxxxxxxxx.
                xxxxxx            :                  '::xxxxxxxxxxxx
               :xxxx:'            :                    'xxxxxxxxxxxx:
              .xxxxx              :                     ::xxxxxxxxxxxx
              xxxx:'                                    ::xxxxxxxxxxxx
              xxxx               .                      ::xxxxxxxxxxxx.
          .:xxxxxx               :                      ::xxxxxxxxxxxx::
          xxxxxxxx               :                      ::xxxxxxxxxxxxx:
          xxxxxxxx               :                      ::xxxxxxxxxxxxx:
          ':xxxxxx               '                      ::xxxxxxxxxxxx:'
            .:. xx:.                                   .:xxxxxxxxxxxxx'
          ::::::.'xx:.            :                  .:: xxxxxxxxxxx':
  .:::::::::::::::.'xxxx.                            ::::'xxxxxxxx':::.
  ::::::::::::::::::.'xxxxx                          :::::.'.xx.'::::::.
  ::::::::::::::::::::.'xxxx:.                       :::::::.'':::::::::
  ':::::::::::::::::::::.'xx:'                     .'::::::::::::::::::::..
    :::::::::::::::::::::.'xx                    .:: :::::::::::::::::::::::
  .:::::::::::::::::::::::. xx               .::xxxx :::::::::::::::::::::::
  :::::::::::::::::::::::::.'xxx..        .::xxxxxxx ::::::::::::::::::::'
  '::::::::::::::::::::::::: xxxxxxxxxxxxxxxxxxxxxxx :::::::::::::::::'
    '::::::::::::::::::::::: xxxxxxxxxxxxxxxxxxxxxxx :::::::::::::::'
        ':::::::::::::::::::_xxxxxx::'''::xxxxxxxxxx '::::::::::::'
             '':.::::::::::'                        `._'::::::''
        """
        Penguins.printing(a, 0.0001)

    @classmethod
    def simple(cls):
        a = """
      .___.
     /     \\
    | O _ O |
    /  \_/  \ 
  .' /     \ `.
 / _|       |_ \\
(_/ |       | \_)
    \       /
   __\_>-<_/__
   ~;/     \;~
        """
        Penguins.printing(a, 0.001)

    @classmethod
    def axOS(cls):
        a = """
        
                                                                            
                                                                            
                                           OOOOOOOOO        SSSSSSSSSSSSSSS 
                                         OO:::::::::OO    SS:::::::::::::::S
                                       OO:::::::::::::OO S:::::SSSSSS::::::S
                                      O:::::::OOO:::::::OS:::::S     SSSSSSS
  aaaaaaaaaaaaa   xxxxxxx      xxxxxxxO::::::O   O::::::OS:::::S            
  a::::::::::::a   x:::::x    x:::::x O:::::O     O:::::OS:::::S            
  aaaaaaaaa:::::a   x:::::x  x:::::x  O:::::O     O:::::O S::::SSSS         
           a::::a    x:::::xx:::::x   O:::::O     O:::::O  SS::::::SSSSS    
    aaaaaaa:::::a     x::::::::::x    O:::::O     O:::::O    SSS::::::::SS  
  aa::::::::::::a      x::::::::x     O:::::O     O:::::O       SSSSSS::::S 
 a::::aaaa::::::a      x::::::::x     O:::::O     O:::::O            S:::::S
a::::a    a:::::a     x::::::::::x    O::::::O   O::::::O            S:::::S
a::::a    a:::::a    x:::::xx:::::x   O:::::::OOO:::::::OSSSSSSS     S:::::S
a:::::aaaa::::::a   x:::::x  x:::::x   OO:::::::::::::OO S::::::SSSSSS:::::S
 a::::::::::aa:::a x:::::x    x:::::x    OO:::::::::OO   S:::::::::::::::SS 
  aaaaaaaaaa  aaaaxxxxxxx      xxxxxxx     OOOOOOOOO      SSSSSSSSSSSSSSS   


        """
        Penguins.printing(a, 0.0005)

    @classmethod
    def amberlab(cls):
        a = """
                                                                                                                                                                                              
                                                        bbbbbbbb                                                                                                  bbbbbbbb            
               AAA                                      b::::::b                                                         LLLLLLLLLLL                              b::::::b            
              A:::A                                     b::::::b                                                         L:::::::::L                              b::::::b            
             A:::::A                                    b::::::b                                                         L:::::::::L                              b::::::b            
            A:::::::A                                    b:::::b                                                         LL:::::::LL                               b:::::b            
           A:::::::::A              mmmmmmm    mmmmmmm   b:::::bbbbbbbbb        eeeeeeeeeeee    rrrrr   rrrrrrrrr          L:::::L                 aaaaaaaaaaaaa   b:::::bbbbbbbbb    
          A:::::A:::::A           mm:::::::m  m:::::::mm b::::::::::::::bb    ee::::::::::::ee  r::::rrr:::::::::r         L:::::L                 a::::::::::::a  b::::::::::::::bb  
         A:::::A A:::::A         m::::::::::mm::::::::::mb::::::::::::::::b  e::::::eeeee:::::eer:::::::::::::::::r        L:::::L                 aaaaaaaaa:::::a b::::::::::::::::b 
        A:::::A   A:::::A        m::::::::::::::::::::::mb:::::bbbbb:::::::be::::::e     e:::::err::::::rrrrr::::::r       L:::::L                          a::::a b:::::bbbbb:::::::b
       A:::::A     A:::::A       m:::::mmm::::::mmm:::::mb:::::b    b::::::be:::::::eeeee::::::e r:::::r     r:::::r       L:::::L                   aaaaaaa:::::a b:::::b    b::::::b
      A:::::AAAAAAAAA:::::A      m::::m   m::::m   m::::mb:::::b     b:::::be:::::::::::::::::e  r:::::r     rrrrrrr       L:::::L                 aa::::::::::::a b:::::b     b:::::b
     A:::::::::::::::::::::A     m::::m   m::::m   m::::mb:::::b     b:::::be::::::eeeeeeeeeee   r:::::r                   L:::::L                a::::aaaa::::::a b:::::b     b:::::b
    A:::::AAAAAAAAAAAAA:::::A    m::::m   m::::m   m::::mb:::::b     b:::::be:::::::e            r:::::r                   L:::::L         LLLLLLa::::a    a:::::a b:::::b     b:::::b
   A:::::A             A:::::A   m::::m   m::::m   m::::mb:::::bbbbbb::::::be::::::::e           r:::::r                 LL:::::::LLLLLLLLL:::::La::::a    a:::::a b:::::bbbbbb::::::b
  A:::::A               A:::::A  m::::m   m::::m   m::::mb::::::::::::::::b  e::::::::eeeeeeee   r:::::r                 L::::::::::::::::::::::La:::::aaaa::::::a b::::::::::::::::b 
 A:::::A                 A:::::A m::::m   m::::m   m::::mb:::::::::::::::b    ee:::::::::::::e   r:::::r                 L::::::::::::::::::::::L a::::::::::aa:::ab:::::::::::::::b  
AAAAAAA                   AAAAAAAmmmmmm   mmmmmm   mmmmmmbbbbbbbbbbbbbbbb       eeeeeeeeeeeeee   rrrrrrr                 LLLLLLLLLLLLLLLLLLLLLLLL  aaaaaaaaaa  aaaabbbbbbbbbbbbbbbb   
                                                                                                                                                                                      
                                                                                                                                                                                      
                                                                                                                                                                                      
        """
        cls.printing(a, 0.000001)

    @classmethod
    def printing(cls, str, delay):
        for c in str:
            print(c, end='')
            time.sleep(delay)