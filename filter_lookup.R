
library(reshape2)
library(ggplot2)
df<-data.frame(route=c(0.5,1,1.5,2),
              
               BLOOM=c(16338760,20161790,23794810,27107540),
               
               CUCKOO=c(15858400,15593830,15473470,15418570))
data.m <- melt(df,id.vars = 'route')
color<-c("#66FF66","#6666FF")
ggplot(data.m, aes(route, value)) + 
  geom_bar(aes(fill = variable), colour="black",
           width = 0.15, position = position_dodge(width=0.2), stat="identity") +  
  
  scale_fill_manual( values=color,labels=c("VPP+BF","VPP+CF")) +
  theme_bw() + 
  theme(legend.title = element_blank(),
        legend.text=element_text(size=15),panel.grid.major = element_blank(), 
        legend.position=c(0.5, 1),
        legend.direction = "horizontal", 
        legend.box = "horizontal",
        legend.background = element_rect(colour = "black"),
        panel.grid.minor = element_blank(),
        panel.border = element_blank(),
        panel.background = element_blank(), axis.line = element_line(colour = "black"),
        axis.text=element_text(size=20),
        axis.title.y=element_text(size = 20),axis.title.x=element_text(size = 20)) + 
  xlab("Route Scale (in Million)")+
  ylab("Count")
  #xlim(0, 2.5) +
  #ylim(0, 4) 




