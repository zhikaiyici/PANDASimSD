#############################输出结果保存###################################
output 文件夹用于存放输出文件

##文本文件(.txt)保存格式：

       |
       |
-------+-------→ y (module row)
       |
       |
       ↓
         z (module)     
            
e.g. 4 × 4

+----+-----+-----+-----+         
| 0  |  1  |  2  |  3  |
+----+-----+-----+-----+
| 4  |  5  |  6  |  7  |
+----+-----+-----+-----+
| 8  |  9  |  10 |  11 |
+----+-----+-----+-----+
| 12 |  13 |  14 |  15 |
+----+-----+-----+-----+


##二进制(.data)文件保存格式：

0 1 2 ... 14 15 0 1 2 ... 14 15 0 ...


#光电子二进制文件中Right和Left文件内容相同，均保存了左右两端的数据：

0R 0L 1R 1L ... 14R 14L 15R 15L 0R 0L ...


#以下数据未按照阵列结构存储(顺序存储)

betaKEHe8, betaKELi9, decayTimeHe8, decayTimeLi9, neutronGT, neutronKE, capTimeH, capTimeGd, muonKEPrimary, neutronKEPrimary

#物理量单位
Time: μs; Energy deposition: MeV; Kenitic energy: keV; Track length: mm
