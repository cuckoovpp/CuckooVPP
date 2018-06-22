
library(reshape2)
library(ggplot2)
df<-data.frame(route=c(0.5,1,1.5,2),
               HASH=c(8195350,8075990,8026410,8007850),
               
               BLOOM=c(692960,953300,1310850,1724740),
               
               CUCKOO=c(564160,616820,664940,704220))
data.m <- melt(df,id.vars = 'route')
color<-c("#FF6666","#66FF66","#6666FF")
ggplot(data.m, aes(route, value)) + 
  geom_bar(aes(fill = variable), colour="black",
           width = 0.3, position = position_dodge(width=0.35), stat="identity") +  
  
  scale_fill_manual( values=color,labels=c("Base VPP","VPP+BF","VPP+CF")) +
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




