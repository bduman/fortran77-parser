# Fortran77 Parser

#### Yakalayabildiği hatalar - Çıktıları ile birlikte

```sh
Its must be digit constant but have alphabetic characters
```
sayısal başlayıp alfabetik giderse

```sh
Its must be operator [logical or relational] but does not exist this language [err1]
```
operator biçiminde olup dile ait olmazsa

```sh
Its must be operator [logical or relational] but does not exist this language [err2]
```
operator gibi başlayıp sonradan nokta ile bitmezse

```sh
Found unidentified character
```
dile ait olmayan karakter tespit edildiği zaman (karakteri de tepesinde basar)

EK : Örnek .for dosyası
EK : programın çalışmasıyla oluşuturlan örnek .txt dosyası

#### Örnek Çıktı

![ornek basarili cikti](https://cloud.githubusercontent.com/assets/5374623/25326862/9e6dae72-28da-11e7-8d56-2fbd8b4b52c5.png)
