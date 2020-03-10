# TEST_AREA_REMOTE_MONITORING


Bu proje: 
--------

1920x1080 bir ekrana 15 adet zone ( text, alfanumerik, gauge1 tiplerinde veri gostergeleri ) oluşturan bir TCP SERVER dan ve buna TCP den data gonderen
client tan oluşmaktadır. 


Projenin amacı:
--------------

Bir olcum sisteminin verilerinin uzak bir Full HD monitorde farkli tipteki ( text, alfanumerik, gauge1 ) zone larla ( max 15 adet ) gosterilmesini saglamaktır.
Bu sistem Raspberry pi 3 B ye hdmi dan takili full hd bir tv ekranına olculen verileri uzaktan yazdirma amaciyla yapildi.


Gelistirme ortami:
-----------------

Ubuntu 16.04 LTS, Qt 5.12.1


Çalıştırma ortami: 
-----------------

Her hangi bir linux ortami. ( Ubuntu 16.04 LTS de ve Raspberry Pi 3 B icerisinde cross-compile ile çalıştırdım. )




Client Send:
-------------------------------------------------------------------------------------------------------------

For Initiliaze: ( example: 2.zone initiliaze )
-------------------------------------------------------------------------------------------------------------

inr:2;tip:gauge1;min:0;max:300;nmin:210;nmax:230;text:Vgiris L1;birim:V AC;visible:true

*inr -> initiliaze zone number 

*tip -> types:
        ----
        text
        alfanumerik
        gauge1

*min     -> minimum value

*max     -> maximum value

*nmin    -> nominal min value ( Yellow text for zone for display )

*nmax    -> nominal max value ( Yellow text for zone for display - After this value, zone's text is red)

*text    -> Zone upperword 

*birim   -> unit

*visible -> true or false. if it is false, zone is disappeared and it's initiliaze is fault.



For Display Data: ( example: 2.zone data )
-------------------------------------------------------------------------------------------------------------

dnr:2;gval1:1;ccount:1 connection;val:235

*dnr    -> zone number for send data

*gval1  -> not used now

*ccount -> connection count (it is constant)

*val    -> value of display
 
