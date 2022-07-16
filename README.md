# multi_lora_tx

> sx1276 tx tress test tool by multiple LoRa nodes.

## Prepare before build

> Build under [openwrt](https://github.com/openwrt/openwrt).
>
> Copy full multi_lora_tx directory to package directory in openwrt.
```bash
cd openwrt
mkdir -p package/extra
cp -rf multi_lora_tx openwrt/package/extra
```
> Add PACKAGE_LORATX section to .config
```bash
cd openwrt
echo "PACKAGE_LORATX=y" >> .config
```
## Build package and firmware.
```bash
cd openwrt
make V=s
```
