### load package
library(ggplot2)

### load data
time_data = read.table("result.txt", header = TRUE, sep = ",", skip = 1)

### plot
attach(time_data)
setEPS()
postscript("result.eps")
ggplot() +
  geom_line(data = time_data, aes(x = time_data$ThreadNumber, y = time_data$TASLock, colour = "TASLock")) +
  geom_line(data = time_data, aes(x = time_data$ThreadNumber, y = time_data$TTASLock, colour = "TTASLock")) +
  geom_line(data = time_data, aes(x = time_data$ThreadNumber, y = time_data$TournamentLock, colour = "TournamentLock")) +
  xlab('Thread Number') + ylab('Time (Nanosecond)')
dev.off()
