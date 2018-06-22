
library(reshape2)
library(ggplot2)
df<-data.frame(route=c(0.5,1,1.5,2),
               HASH=c(3.355704698,3.2679738562,3.0674846626,3.0487804878),
               BLOOM=c(3.8759689922,3.5211267606,3.164556962,2.9069767442),
               TRIE=c(1.6722408027,1.8115942029,1.8656716418,1.8518518519),
               CUCKOO=c(3.8167938931,3.6764705882,3.6496350365,3.7878787879))
data.m <- melt(df,id.vars = 'route')
color<-c("#FF6666","#66FF66","#FFFF66","#6666FF")
ggplot(data.m, aes(route, value)) + 
  geom_bar(aes(fill = variable), colour="black",
   width = 0.3, position = position_dodge(width=0.35), stat="identity") +  

  scale_fill_manual( values=color,labels=c("Base VPP", "VPP+BF", "VPP+TRIE","VPP+CF")) +
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
  ylab("Lookup Rate (MLPS)")+
  #xlim(0, 2.5) +
  ylim(0, 4) 
  
