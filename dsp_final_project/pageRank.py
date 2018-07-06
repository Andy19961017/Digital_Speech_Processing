import numpy as np
import gensim as gs

alpha=0.1
dim=100
print_top_num=10
first_pass_num=100
database_size=100000

queries=[ '希望每天都是星期天',
          '海邊的風很大',
          '白日依山盡 黃河入海流',
          '數位語音好好玩',
          '我覺得助教長得滿帥的',
          '我暑假想要騎腳踏車環島',
          '上次那個女明星真的很辣',
          '期末考壓力好大',
          '今天天氣真好',
          '我中午想吃便當',
          '我要成為絕地武士',]

def cos_sim(a,b):
  sim = np.dot(a, b) / np.linalg.norm(a) / np.linalg.norm(b)
  if sim > 0: return sim
  else: return 0

def compute_avg_vecs(corpus):
  avg_vecs=[]
  for s in corpus:
    emb_cnt = 0
    avg_sent_emb = np.zeros((dim,))
    for word in s:
      if word in word_vecs.wv.vocab:
        emb_cnt += 1
        avg_sent_emb += word_vecs[word]
    avg_sent_emb /= emb_cnt
    avg_vecs.append(avg_sent_emb)
  return np.array(avg_vecs)

def simRank(query, corpus, avg_vecs):
  emb_cnt = 0
  avg_query_emb = np.zeros((dim,))
  for word in query:
    if word in word_vecs.wv.vocab:
      avg_query_emb += word_vecs[word]
      emb_cnt += 1
  avg_query_emb /= emb_cnt

  sim=[]
  for vec in avg_vecs:
    sim.append(cos_sim(avg_query_emb, vec))
  sim=np.array(sim)

  sim_corpus=np.concatenate((sim.reshape(-1,1),corpus.reshape(-1,1),avg_vecs),axis=1)
  sim_corpus=sim_corpus[sim_corpus[:,0].argsort()]
  sim_corpus=sim_corpus[-100:]
  sim_corpus[:,0]=sim_corpus[:,0].astype(float)/np.sum(sim_corpus[:,0].astype(float))
  return sim_corpus

def pageRank(first_pass):
  n=first_pass.shape[0]
  original_sim=first_pass[:,0].astype(float)
  vecs=first_pass[:,2:].astype(float)
  P=np.zeros((n,n))
  for i in range(n):
    sim=[]
    for j in range(n):
      sim.append(cos_sim(vecs[i],vecs[j]))
    sim=np.array(sim)
    sim/=np.sum(sim)
    P[i]=sim
  r=np.concatenate((original_sim.reshape(-1,1),)*n, axis=1)
  P1=(1-alpha)*r+alpha*P.T

  w,v=np.linalg.eig(P1)
  V=v[:,np.argmax(w)]
  V/=np.sum(V)

  second_pass=np.concatenate((V.reshape(-1,1),first_pass[:,1].reshape(-1,1)),axis=1)
  second_pass=second_pass[second_pass[:,0].argsort()]
  return(second_pass[-print_top_num:,1])



word_vecs=gs.models.Word2Vec.load('corpus.model')
corpus = np.load('corpus.npy')[:database_size]
print(len(corpus),'sentences loaded in database.')
avg_vecs=compute_avg_vecs(corpus)

for query in queries:
  print('query:',query)
  print(' {:28} {:20}'.format('Ranked by cos_sim', 'Reranking'))
  first_pass=simRank(query, corpus, avg_vecs)
  second_pass=pageRank(first_pass)
  for x in range(1,print_top_num+1):
    print('{:2} {:20} {:20}'.format(x, first_pass[-x,1], second_pass[-x]))
  print()
