<div align="center">

# Byte-knight — Arch Linux Fork

**Arch Linux paket deposu — [Neuwj](https://github.com/Neuwj-00)/byte-knight projesinin Arch fork'u**

![GPL v3](https://img.shields.io/badge/License-GPL_v3-blue?style=for-the-badge&logo=gnu&logoColor=white)
![Arch Linux](https://img.shields.io/badge/Arch_Linux-1793D1?style=for-the-badge&logo=archlinux&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Rust](https://img.shields.io/badge/Rust-000000?style=for-the-badge&logo=rust&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

</div>

---

## Hakkinda

Bu depo, [Neuwj-00/byte-knight](https://github.com/Neuwj-00/byte-knight) projesinin **Arch Linux** dagitimina yonelik fork'udur. Amaci, orijinal projede yer alan yuksek performansli C++ terminal araclarini Arch Linux ve turevi dagitimlar (Manjaro, EndeavourOS, Garuda vb.) icin paketlemek ve dagitmaktir.

Orijinal proje Debian (`.deb`) ve Red Hat (`.rpm`) paketlerini desteklerken, bu fork Arch Linux ekosistemine odaklanir.

---

## Dahil Edilen Araclar

- **pacForge** — Evrensel Linux paketleme sihirbazi. `.deb` ve `.rpm` paketlerinin olusturulmasini otomatiklestirir.
- **repoForge** — Yerel veya uzak APT/RPM paket depolari olusturmak ve yonetmek icin ozel bir arac.
- **foldertree (ftr)** — Dizin yapisini gorsellestirir ve agac formatinda yazdiran ozellestirilebilir bir CLI araci.
- **warp / warpcplus** — Ag islemleri, tunelleme ve baglanti yonetimi icin tasarlanmis bir C++ araci.

---

## Kurulum (Arch Linux)

### Kaynaktan Derleme

```bash
# Depoyu klonlayin
git clone https://github.com/Vniverse77/byte-knight.git
cd byte-knight/src

# g++ ile derleyin (ornek: foldertree)
g++ -std=c++17 -O2 -Wl,-z,relro,-z,now -o ftr foldertree.cpp

# Derlenmis ikili dosyayi sisteme yukleyin
sudo install -Dm755 ftr /usr/bin/ftr
```

### Diger Dagitimlar

Debian/Ubuntu ve Fedora/RHEL kurulumu icin orijinal projeye basvurun: [Neuwj-00/byte-knight](https://github.com/Neuwj-00/byte-knight)

---

## Depo Yapisi

```
byte-knight/
├── src/                   # Kaynak kodlar
│   ├── foldertree.cpp
│   ├── pacForge.cpp
│   ├── repoForge.cpp
│   ├── warp.cpp
│   ├── gitForge.rs
│   ├── uninstallerdeb.py
│   └── RepositoryInstaller.sh
├── keys/                  # GPG imzalama anahtarlari
├── index.html             # Web sayfasi
├── LICENSE                # GNU GPLv3 Lisans
└── README.md
```

---

## Katki

Katkilar, hata bildirimleri ve ozellik istekleri memnuniyetle karsilanir! [Issues](https://github.com/Vniverse77/byte-knight/issues) sayfasindan bildirimde bulunabilirsiniz.

---

## Iletisim

- **Fork sahibi:** [Vniverse77](https://github.com/Vniverse77)
- **Orijinal gelistirici:** [Neuwj](https://github.com/Neuwj-00)
- **Orijinal proje:** [neuwj-00.github.io/byte-knight](https://neuwj-00.github.io/byte-knight/)

---

## Lisans

Bu proje [GNU General Public License v3.0](LICENSE) lisansi altinda acik kaynaklidir.

Orijinal proje [Neuwj](https://github.com/Neuwj-00) tarafindan olusturulmustur.
