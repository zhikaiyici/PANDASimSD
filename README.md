# PANDASimSD: A GEANT4 simulation of plastic scintillator antineutrino detector array

## Requirements
- **GEANT4: `11.2.1`**
- **CRY: `1.7`**

##  运行方式
1. 编译后直接执行可执行文件以UI模式运行，此时将自动加载`init_vis.mac`
2. 编译后执行可执行文件，并输入`*.mac`文件以批处理模式运行，`*.mac`文件最多为2个，见***自定义宏命令***

### 输入能谱
`spectra`文件夹用于存放自定义能谱文件，能谱文件格式

```
能量(MeV)  在能谱中的相对强度
```

- 示例：能量为0.0253 eV 单能中子

```
   2.53E-08  1
```

### 输出结果保存
`output`文件夹用于存放输出文件
  - 保存格式：

    e.g. 4 × 4
    
    ![alt text](array.png)

- 二进制(.data)文件保存格式：

  > 0 1 2 ... 14 15 0 1 2 ... 14 15 0 ...

1. 光电子二进制文件中Right和Left文件内容相同，均保存了左右两端的数据：

   > 0R 0L 1R 1L ... 14R 14L 15R 15L 0R 0L ...

2. 以下数据未按照阵列结构存储(顺序存储)

   > betaKEHe8, betaKELi9, decayTimeHe8, decayTimeLi9, neutronGT, neutronKE, capTimeH, capTimeGd, muonKEPrimary, neutronKEPrimary

3. 物理量单位
   > Time: `μs`  
   > Energy deposition: `MeV`  
   > Kenitic energy: `keV`  
   > Primary kenitic energy: `GeV`
   > Track length: `mm`

## 自定义宏命令

### 1. detector 

> Use this command ***before*** `/run/initialize`
   
```
/detector/arraySize
/detector/detectorX
/detector/detectorY
/detector/detectorZ
/detector/moduleDistance
/detector/gdFilmThickness
/detector/addLabRoom
```
 <!--
 /detector/update
```
-->

- `/detector/arraySize`
  - Detector array size, a positive integer.
  - Default value: `4`.

- `/detector/detectorX`
  - Detector X dimension, a positive double with unit (`cm` by default).
  - Default value: `10. cm`.

- `/detector/detectorY`
  - Detector Y dimension, a positive double with unit (`cm` by default).
  - Default value: `10. cm`.

- `/detector/detectorZ`
  - Detector Z dimension, a positive double with unit (`cm` by default).
  - Default value: `100. cm`.

- `/detector/moduleDistance`
  - Distance between modules, a positive double with unit (`cm` by default).
  - Default value: `2. cm`.

- `/detector/gdFilmThickness`
  - Gd film thickness, a positive double with unit (`um`, μm, by default).
  - Default value: `30. um`.

- `/detector/addLabRoom`
  - Available values: `true`/`false`.
  - Adding/removing lab room.
  - Default value: `true`.

<!--
- `/detector/update`
  - Updating geometry after changing it, no parameter.
  - **DONOT USE!! BUG WITH SENSITIVE DETECTOR.**
-->

### 2. source

> Use this command ***after*** `/run/initialize`

```
/source/type              
/source/spectra     
/source/position          
/source/neutrinoPosition
```

- `/source/type`
  - Available values:  
    `Cs137`/`Co60`/`Na22`/`Cs137g`/`Co60g`/`Am-Be-n`/`GUN`/`He8`/`Li9`/`NEUTRINO`/`MUON`/`COSMICNEUTRON`/`CRY` (Linux only)/`GPS`.
  - `Cs137`/`Co60`/`Na22`/`Cs137g`/`Co60g`/`Am-Be-n`/`GUN` with position `CENTER`/`EDGE`;  
    `GUN` with position `INSIDE` or `COMMAND` (means defined by UI command `/gun/position`). Energy, momentum, particle etc. must be defined with source `GUN` using UI command `/gun/**`.  
    Position, momentum, particle etc. must be defined with source `GPS` using UI command `/gps/**`.
  - Default value: `NEUTRINO`.

- `/source/spectra`
  - Spectra of neutron and positron, two strings at most. See spectra directory.  
    First one should be spectrum of neutron.  
    Second one, if there is, should be spectrum of positron.
  - Command is valid only for `NEUTRINO` and `COSMICNEUTRON`.
  - Default value: `IBDNeutron.spec IBDPositron.spec`.

- `/source/position`
  - Available values:  
    `CENTER`/`EDGE`/`INSIDE`/`COMMAND`
  - Position for `Cs137`/`Co60`/`Na22`/`Cs137g`/`Co60g`/`Am-Be-n`/`GUN`  
    Direction must be defined for `GUN` with position `CENTER`/`EDGE`.  
    Direction and position must be defined for `GUN` with position `COMMAND`.  
    Direction is isotropic for `GUN` with position `INSIDE`.
  - Default value: `CENTER`.

- `/source/neutrinoPosition`
  - Position for `NEUTRINO`, two positive integers [0 - size × size, 0 - 5].  
    First integer is position in the detector array, size × size means random.  
    Second integer is position in the detector module, 5 means random.  
    Second integer is valid only when arrayPosition < size × size.
  - Default value: `size × size 5`.

### 3. physics

> Use this command in the ***second*** input `*.mac` file, the first should be the one with `/run/initialize` and `/run/beamOn`.  
  This `*.mac` file is ***optional***. Without this file optical process and muonic atom decay process will be ***off***. 

```
/physics/optical
/physics/muonicDecay
```

- `/physics/optical`
  - Available values: `true`/`false`.
  - Turning on/off optical process.
  - Default value: `true`.

- `/physics/muonicDecay`
  - Available values: `true`/`false`.
  - Turning on/off muonic atom decay process.
  - Default value: `true`.

### 4. time

> Use this command ***after*** `/run/initialize`

```
/time/timeInterval
```

- `/time/timeInterval`
  - Time interval between parent particle and secondaries, a positive double with unit (`us`,  μs, by default).
  - If time interval between parent particle and secondaries is bigger than this value, they will be treated as two signals.
  - Default value: `1. us`.