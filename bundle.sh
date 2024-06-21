#!/bin/sh

w4 bundle --windows mazethingie-windows.exe  --title Mazethingie build/cart.wasm
w4 bundle --mac mazethingie-mac  --title Mazethingie build/cart.wasm
w4 bundle --linux mazethingie-linux  --title Mazethingie build/cart.wasm
w4 bundle --html index.html  --title Mazethingie build/cart.wasm
cp build/cart.wasm mazethingie.wasm
chmod +x ./mazethingie-mac
chmod +x ./mazethingie-linux
chmod +x ./mazethingie-windows.exe
tar -czvf ./mazethingie-windows.tar.gz mazethingie-windows.exe
tar -czvf ./mazethingie-mac.tar.gz mazethingie-mac
tar -czvf ./mazethingie-linux.tar.gz mazethingie-linux
tar -czvf ./mazethingie-html.tar.gz index.html
tar -czvf ./mazethingie-cart.tar.gz mazethingie.wasm
rm -f ./mazethingie-windows.exe
rm -f ./mazethingie-mac
rm -f ./mazethingie-linux
