FROM sagemath/sagemath:9.6

USER root
COPY solver.sage .

RUN sage --pip install pycryptodome pwntools tqdm logger
CMD ["sage", "solver.sage"]
