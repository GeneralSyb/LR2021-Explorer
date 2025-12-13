# LR2021-Explorer
A small development board for Semtech's LR2021 4th generation LoRa transceiver.
<img width="1386" height="1818" alt="image" src="https://github.com/user-attachments/assets/8f662a51-0489-461c-b17f-2061d2f3049e" />
<img width="977" height="1479" alt="image" src="https://github.com/user-attachments/assets/d8fd1a40-9d3d-479c-8774-a9362fc4142e" />

## References
This design is based on the publicly available reference design available at:
https://www.semtech.com/products/wireless-rf/lora-plus/lr2021
The design uses a direct-tie implementation, which means there is no RF switch to switch between RX and TX mode. The reduces the component count, possibly at the loss of some performance.

## Testing
There is no Arduino-style library available yet, but some people are working on adding support for the chip in the RadioLib library. Therefore it is not know if this board actually works or not.
