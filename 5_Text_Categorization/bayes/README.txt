说明：
本目录下的t_bayes.py为源代码，report.xlsx为报告，data文件夹下为被提供的训练数据，README.txt为说明文件

执行python t_bayes.py即可执行基于朴素贝叶斯的机器学习的代码

若执行
python t_bayes.py
则将采取十折交叉验证的方法运行，并将十次【学习+测试】的训练集信息和测试集信息（包括正确分类和测试结果一一对应）输出到output.csv的表格中，
在命令行将输出测试集中四种类别文档的整体正确率

若执行
python t_bayes.py [path]
则将对所有数据进行学习，并基于学习的结果对path路径的文件进行预测判断，然后在命令行输出结果，并将训练集数据输出到output.csv的表格中

report.xslx为报告，是将某一次运行得到的csv输出文件进过整理之后得到的数据